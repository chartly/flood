/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "Graphics/API.h"
#include "Graphics/HalfEdgeMesh.h"
#include "Core/Memory.h"

namespace dit {

    using namespace mesh;

    //////////////////////////////////////////////////////////////////////////

    VertexFaceNode::VertexFaceNode(uint32 next_, uint32 faceIdx_)
        : next(next_)
        , faceIdx(faceIdx_)
    {}

    FaceEdgeNode::FaceEdgeNode(uint32 next_, uint32 edgeIdx_)
        : next(next_)
        , edgeIdx(edgeIdx_)
    {}

    HalfEdge::HalfEdge(uint32 twinIdx_, uint32 faceIdx_, std::array<uint32, 2> vtx_)
        : twinIdx(twinIdx_)
        , faceIdx(faceIdx_)
        , vtx(std::move(vtx_))
    {}

    //////////////////////////////////////////////////////////////////////////

    HalfEdgeMesh::HalfEdgeMesh(HalfEdgeMesh&& rhs)
        : vertexPositions(std::move(rhs.vertexPositions))
        , vertexNormals(std::move(rhs.vertexNormals))
        , vertexFaces(std::move(rhs.vertexFaces))
        , vfNodes(std::move(rhs.vfNodes))
        , faceVertices(std::move(rhs.faceVertices))
        , faceEdges(std::move(rhs.faceEdges))
        , feNodes(std::move(rhs.feNodes))
        , halfEdges(std::move(rhs.halfEdges))
    {}

    //////////////////////////////////////////////////////////////////////////

    MeshIterator::MeshIterator(Context ctx_, uint32 idx_, HalfEdgeMesh* mesh_)
        : ctx(ctx_)
        , idx(idx_)
        , mesh(std::move(mesh_))
    {}

    MeshIterator MeshIterator::nextVertex(const MeshIterator*) const
    {
        return MeshIterator{};
    }

    MeshIterator MeshIterator::nextEdge(const MeshIterator*) const
    {
        return MeshIterator{};
    }

    MeshIterator MeshIterator::nextFace(const MeshIterator*) const
    {
        return MeshIterator{};
    }

    //////////////////////////////////////////////////////////////////////////

    VertexIterator::VertexIterator(Context ctx, uint32 idx, HalfEdgeMesh* mesh)
        : MeshIterator(ctx, idx, std::move(mesh))
    {}

    vec3& VertexIterator::pos()
    {
        return mesh->vertexPositions[idx];
    }

    const vec3& VertexIterator::pos() const
    {
        return mesh->vertexPositions[idx];
    }

    vec3& VertexIterator::normal()
    {
        return mesh->vertexNormals[idx];
    }

    const vec3& VertexIterator::normal() const
    {
        return mesh->vertexNormals[idx];
    }

    MeshIterator VertexIterator::nextVertex(const MeshIterator* callee) const
    {
        assert(callee != nullptr);
        assert(ctx.data != MeshNullIdx);

        if (idx != MeshNullIdx)
            return ctx.parent->nextVertex(this);

        auto nextIdx = callee->idx + 1;
        if (nextIdx < ctx.data)
            return MeshIterator{ callee, nextIdx, mesh };

        return MeshIterator{};
    }

    MeshIterator VertexIterator::nextFace(const MeshIterator* callee) const
    {
        assert(callee != nullptr);

        auto nodeIdx = mesh->vfNodes[callee->idx].next;
        auto callee_ = (MeshIterator*)callee;
        callee_->idx = nodeIdx;

        return MeshIterator{ callee, mesh->vfNodes[nodeIdx].faceIdx, mesh };
    }

    MeshIterator VertexIterator::nextEdge(const MeshIterator*) const
    {
        return MeshIterator::nextEdge(nullptr);
    }

    //////////////////////////////////////////////////////////////////////////

    EdgeIterator::EdgeIterator(Context ctx, uint32 idx, HalfEdgeMesh* mesh)
        : MeshIterator(ctx, idx, std::move(mesh))
    {}

    HalfEdge& EdgeIterator::value()
    {
        return mesh->halfEdges[idx];
    }

    const HalfEdge& EdgeIterator::value() const
    {
        return mesh->halfEdges[idx];
    }

    MeshIterator EdgeIterator::nextVertex(const MeshIterator*) const
    {
        return MeshIterator::nextVertex(nullptr);
    }

    MeshIterator EdgeIterator::nextEdge(const MeshIterator* callee) const
    {
        assert(callee != nullptr);
        assert(ctx.data != MeshNullIdx);

        if (idx != MeshNullIdx)
            return ctx.parent->nextEdge(this);

        auto nextIdx = callee->idx + 1;
        if (nextIdx < ctx.data)
            return MeshIterator{ callee, nextIdx, mesh };

        return MeshIterator{};
    }

    MeshIterator EdgeIterator::nextFace(const MeshIterator*) const
    {
        return MeshIterator::nextFace(nullptr);
    }

    //////////////////////////////////////////////////////////////////////////

    FaceIterator::FaceIterator(Context ctx, uint32 idx, HalfEdgeMesh* mesh)
        : MeshIterator(ctx, idx, std::move(mesh))
    {}

    Face& FaceIterator::value()
    {
        return mesh->faceVertices[idx];
    }

    const Face& FaceIterator::value() const
    {
        return mesh->faceVertices[idx];
    }

    MeshIterator FaceIterator::nextVertex(const MeshIterator* callee) const
    {
        assert(callee != nullptr);

        auto nextIdx = callee->idx + 1;
        if (nextIdx > 2)
            return MeshIterator{};

        auto callee_ = (MeshIterator*)callee;
        callee_->idx = nextIdx;

        return MeshIterator{ callee, mesh->faceVertices[idx].vtx[nextIdx], mesh };
    }

    MeshIterator FaceIterator::nextEdge(const MeshIterator* callee) const
    {
        assert(callee != nullptr);

        auto nodeIdx = mesh->feNodes[callee->idx].next;
        if (nodeIdx == MeshNullIdx)
            return MeshIterator{};

        auto callee_ = (MeshIterator*)callee;
        callee_->idx = nodeIdx;

        return MeshIterator{ callee, mesh->feNodes[nodeIdx].edgeIdx, mesh };
    }

    MeshIterator FaceIterator::nextFace(const MeshIterator* callee) const
    {
        assert(callee != nullptr);
        assert(ctx.data != MeshNullIdx);

        if (idx != MeshNullIdx)
            return ctx.parent->nextFace(this);

        auto nextIdx = callee->idx + 1;
        if (nextIdx < ctx.data)
            return MeshIterator{ this, nextIdx, mesh };

        return MeshIterator{};
    }

    //////////////////////////////////////////////////////////////////////////

    namespace mesh {

        VertexIterator vertices(HalfEdgeMesh* mesh)
        {
            auto vtxCount = mesh->vertexPositions.size();
            return VertexIterator{ vtxCount, MeshNullIdx, mesh };
        }

        VertexIterator vertices(const FaceIterator& face)
        {
            return VertexIterator{ &face, face.mesh->faceVertices[face.idx].vtx[0], face.mesh };
        }

        //////////////////////////////////////////////////////////////////////////

        EdgeIterator edges(HalfEdgeMesh* mesh)
        {
            auto edgeCount = mesh->halfEdges.size();
            return EdgeIterator{ edgeCount, MeshNullIdx, mesh };
        }

        EdgeIterator edges(const FaceIterator& face)
        {
            auto nodeIdx = face.mesh->faceEdges[face.idx].nodeIdx[0];
            return EdgeIterator{ &face, nodeIdx, face.mesh };
        }

        EdgeIterator twin(const EdgeIterator& edge)
        {
            return EdgeIterator{ MeshNullIdx, edge.value().twinIdx, edge.mesh };
        }

        //////////////////////////////////////////////////////////////////////////

        FaceIterator faces(const VertexIterator& vtx)
        {
            auto nodeIdx = vtx.mesh->vertexFaces[vtx.idx].nodeIdx;
            return FaceIterator{ &vtx, nodeIdx, vtx.mesh };
        }

        FaceIterator faces(HalfEdgeMesh* mesh)
        {
            auto faceCount = mesh->faceVertices.size();
            return FaceIterator{ faceCount, MeshNullIdx, mesh };
        }

        //////////////////////////////////////////////////////////////////////////

        VertexIterator begin(VertexIterator& verts)
        {
            uint32 idx = 0;
            if (verts.idx != MeshNullIdx)
            {
                idx = verts.idx;
                verts.idx = 0;
            }

            return VertexIterator{ &verts, idx, verts.mesh };
        }

        EdgeIterator begin(EdgeIterator& edges)
        {
            uint32 idx = 0;
            if (edges.idx != MeshNullIdx)
                idx = edges.mesh->feNodes[edges.idx].edgeIdx;

            return EdgeIterator{ &edges, idx, edges.mesh };
        }

        FaceIterator begin(FaceIterator& faces)
        {
            uint32 idx = 0;
            if (faces.idx != MeshNullIdx)
            {
                auto nodeIdx = faces.idx;
                faces.idx = faces.mesh->vfNodes[nodeIdx].next;
                idx = faces.mesh->vfNodes[nodeIdx].faceIdx;
            }

            return FaceIterator{ &faces, idx, faces.mesh };
        }

        //////////////////////////////////////////////////////////////////////////

        VertexIterator next(const VertexIterator& vtx)
        {
            if (vtx.ctx.data == MeshNullIdx)
                return end(vtx);

            auto nxt = vtx.ctx.parent->nextVertex(&vtx);
            return VertexIterator{ nxt.ctx, nxt.idx, nxt.mesh };
        }

        EdgeIterator next(const EdgeIterator& edge)
        {
            if (edge.ctx.data == MeshNullIdx)
                return end(edge);

            auto nxt = edge.ctx.parent->nextEdge(&edge);
            return EdgeIterator{ nxt.ctx, nxt.idx, nxt.mesh };
        }

        FaceIterator next(const FaceIterator& face)
        {
            if (face.ctx.data == MeshNullIdx)
                return end(face);

            auto nxt = face.ctx.parent->nextFace(&face);
            return FaceIterator{ nxt.ctx, nxt.idx, nxt.mesh };
        }

        //////////////////////////////////////////////////////////////////////////

        const MeshIterator EndIter = MeshIterator{ MeshNullIdx, MeshNullIdx, nullptr };

        bool null(const VertexIterator& vtx)
        {
            return (vtx.ctx.data == EndIter.ctx.data && vtx.mesh == EndIter.mesh && vtx.idx == EndIter.idx);
        }

        bool null(const EdgeIterator& edge)
        {
            return (edge.ctx.data == EndIter.ctx.data && edge.mesh == EndIter.mesh && edge.idx == EndIter.idx);
        }

        bool null(const FaceIterator& face)
        {
            return (face.ctx.data == EndIter.ctx.data && face.mesh == EndIter.mesh && face.idx == EndIter.idx);
        }

        //////////////////////////////////////////////////////////////////////////

        VertexIterator end(const VertexIterator&)
        {
            return VertexIterator{ MeshNullIdx, MeshNullIdx, nullptr };
        }

        EdgeIterator end(const EdgeIterator&)
        {
            return EdgeIterator{ MeshNullIdx, MeshNullIdx, nullptr };
        }

        FaceIterator end(const FaceIterator&)
        {
            return FaceIterator{ MeshNullIdx, MeshNullIdx, nullptr };
        }

        //////////////////////////////////////////////////////////////////////////

        void walk(VertexIterator vertices, VertexProcessor func)
        {
            auto vtx = begin(vertices);
            while (!null(vtx))
                vtx = func(vtx);
        }

        void walk(EdgeIterator edges, EdgeProcessor func)
        {
            auto edge = begin(edges);
            while (!null(edge))
                edge = func(edge);
        }

        void walk(FaceIterator faces, FaceProcessor func)
        {
            auto face = begin(faces);
            while (!null(face))
                face = func(face);
        }
    }
}