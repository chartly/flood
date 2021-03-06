﻿
using System;

namespace Flood.RPC.Serialization
{
    public interface IDataObject
    {
        void Read(RPCData data, Type baseType = null);
        void Write(RPCData data, Type baseType = null);

        unsafe void Write(RPCData data, BitField* bitFields, int bitFieldCount);
        int BaseDataObjectCount { get; }
    }

    public interface IObservableDataObject : IDataObject
    {
        event Action<Type, int> PropertyChanged;
        unsafe void GetResetChanges(BitField* bitFields);
        bool IsReference { get; set; }
    }

    public interface IDataObjectReference : IObservableDataObject
    {
        RPCPeer Peer { get; }
        int RemoteId { get; }
        ReferenceManager ReferenceManager { get; }
    }
}
