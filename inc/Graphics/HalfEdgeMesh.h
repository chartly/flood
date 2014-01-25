/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "Graphics/API.h"
#include "Core/Object.h"

namespace dit {

    //////////////////////////////////////////////////////////////////////////

    const uint32 MeshNullIdx = std::numeric_limits<uint32>::max();

    struct VertexFace
    {
        uint32 nodeIdx = MeshNullIdx;
    };

    struct VertexFaceNode
    {
        VertexFaceNode() = default;
        VertexFaceNode(uint32 next, uint32 faceIdx);

        uint32 next = MeshNullIdx;
        uint32 faceIdx = MeshNullIdx;
    };

    struct Face
    {
        std::array<uint32, 3> vtx = { { MeshNullIdx, MeshNullIdx, MeshNullIdx } };
    };

    struct FaceEdge
    {
        std::array<uint32, 3> nodeIdx = { { MeshNullIdx, MeshNullIdx, MeshNullIdx } };
    };

    struct FaceEdgeNode
    {
        FaceEdgeNode() = default;
        FaceEdgeNode(uint32 next, uint32 edgeIdx);

        uint32 next = MeshNullIdx;
        uint32 edgeIdx = MeshNullIdx;
    };

    struct HalfEdge
    {
        HalfEdge() = default;
        HalfEdge(uint32 twinIdx, uint32 faceIdx, std::array<uint32, 2> vtx);

        uint32 twinIdx = MeshNullIdx;    // index of the twin edge 
        uint32 faceIdx = MeshNullIdx;    // index into the face index list
        std::array<uint32, 2> vtx = { { MeshNullIdx, MeshNullIdx } };  // indices into the vertex list
    };

    struct HalfEdgeHash
    {
        union
        {
            uint32 v[2];
            uint64 key;
        };
    };

    //////////////////////////////////////////////////////////////////////////

    class ALIGN_BEGIN(32) HalfEdgeMesh : public ReferenceCounted
    {
    public:
        HalfEdgeMesh() = default;
        HalfEdgeMesh(const HalfEdgeMesh&) = delete;
        HalfEdgeMesh(HalfEdgeMesh&& rhs);

        Array<vec3> vertexPositions;
        Array<vec3> vertexNormals;
        Array<VertexFace> vertexFaces;
        Array<VertexFaceNode> vfNodes;

        Array<Face> faceVertices;
        Array<FaceEdge> faceEdges;
        Array<FaceEdgeNode> feNodes;

        Array<HalfEdge> halfEdges;
    } ALIGN_END(32);

    typedef RefPtr<HalfEdgeMesh> HalfEdgeMeshPtr;

    //////////////////////////////////////////////////////////////////////////

    class MeshIterator
    {
    public:
        struct Context
        {
            union
            {
                const MeshIterator* parent;
                uint32 data;
            };

            Context() : data(MeshNullIdx) {}
            Context(uint32 val) : data(val) {}
            Context(const MeshIterator* ptr) : parent(ptr) {}
        };

    public:
        MeshIterator() = default;
        MeshIterator(Context ctx, uint32 idx, HalfEdgeMesh* mesh);

        virtual MeshIterator nextVertex(const MeshIterator*) const;
        virtual MeshIterator nextEdge(const MeshIterator*) const;
        virtual MeshIterator nextFace(const MeshIterator*) const;

    public:
        Context ctx = { MeshNullIdx };
        uint32  idx = MeshNullIdx;
        HalfEdgeMesh* mesh = nullptr;
    };

    class VertexIterator : public MeshIterator
    {
    public:
        VertexIterator(Context ctx, uint32 idx, HalfEdgeMesh* mesh);

        vec3& pos();
        const vec3& pos() const;

        vec3& normal();
        const vec3& normal() const;

    private:
        virtual MeshIterator nextVertex(const MeshIterator* callee) const;
        virtual MeshIterator nextFace(const MeshIterator* callee) const;
        virtual MeshIterator nextEdge(const MeshIterator*) const;
    };

    class EdgeIterator : public MeshIterator
    {
    public:
        EdgeIterator(Context ctx, uint32 idx, HalfEdgeMesh* mesh);

        HalfEdge& value();
        const HalfEdge& value() const;

    private:
        virtual MeshIterator nextVertex(const MeshIterator*) const;
        virtual MeshIterator nextEdge(const MeshIterator* callee) const;
        virtual MeshIterator nextFace(const MeshIterator*) const;
    };

    class FaceIterator : public MeshIterator
    {
    public:
        FaceIterator(Context ctx, uint32 idx, HalfEdgeMesh* mesh);

        Face& value();
        const Face& value() const;

    private:
        virtual MeshIterator nextVertex(const MeshIterator* callee) const;
        virtual MeshIterator nextEdge(const MeshIterator* callee) const;
        virtual MeshIterator nextFace(const MeshIterator* callee) const;
    };

    //////////////////////////////////////////////////////////////////////////

    namespace mesh {

        VertexIterator vertices(HalfEdgeMesh* mesh);
        VertexIterator vertices(const FaceIterator& face);

        EdgeIterator edges(HalfEdgeMesh* mesh);
        EdgeIterator edges(const FaceIterator& face);
        EdgeIterator twin(const EdgeIterator& edge);

        FaceIterator faces(const VertexIterator& vtx);
        FaceIterator faces(HalfEdgeMesh* mesh);

        VertexIterator begin(VertexIterator& vertices);
        EdgeIterator begin(EdgeIterator& edges);
        FaceIterator begin(FaceIterator& faces);

        VertexIterator next(const VertexIterator& vtx);
        EdgeIterator next(const EdgeIterator& edge);
        FaceIterator next(const FaceIterator& face);

        bool null(const VertexIterator& vtx);
        bool null(const EdgeIterator& edge);
        bool null(const FaceIterator& face);

        VertexIterator end(const VertexIterator&);
        EdgeIterator end(const EdgeIterator&);
        FaceIterator end(const FaceIterator&);

        //////////////////////////////////////////////////////////////////////////

        typedef std::function<VertexIterator(VertexIterator&)> VertexProcessor;
        typedef std::function<EdgeIterator(EdgeIterator&)> EdgeProcessor;
        typedef std::function<FaceIterator(FaceIterator&)> FaceProcessor;

        void walk(VertexIterator vertices, VertexProcessor func);
        void walk(EdgeIterator edges, EdgeProcessor func);
        void walk(FaceIterator faces, FaceProcessor func);
    }
}