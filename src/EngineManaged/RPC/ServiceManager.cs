﻿using System;
using System.Collections.Generic;
using System.Reflection;
using System.Threading;
using Flood.RPC.Metadata;

namespace Flood.RPC
{
    public class ServiceManager
    {
        private Dictionary<int, RPCStub> stubs;
        private int stubIdCounter;

        private Dictionary<object, RPCImpl> impls;
        private Dictionary<Tuple<RPCPeer, int>, RPCProxy> proxies;

        private Dictionary<Tuple<RPCPeer, GlobalServiceId>, RPCProxy> globalServiceProxies;
        private Dictionary<GlobalServiceId, int> localGlobalServiceIds;
        private Dictionary<int, GlobalServiceId> stubIdToGlobalServiceId;

        private RPCManager RPCManager;

        public ServiceManager(RPCManager rpcMananger)
        {
            stubs = new Dictionary<int, RPCStub>();
            impls = new Dictionary<object, RPCImpl>();
            proxies = new Dictionary<Tuple<RPCPeer, int>, RPCProxy>();
            globalServiceProxies = new Dictionary<Tuple<RPCPeer, GlobalServiceId>, RPCProxy>();
            localGlobalServiceIds = new Dictionary<GlobalServiceId, int>();
            stubIdToGlobalServiceId = new Dictionary<int, GlobalServiceId>();

            RPCManager = rpcMananger;
        }

        internal void Process(RPCData data)
        {
            var stubId = data.Header.LocalId;

            if(data.Header.CallType == RPCDataType.Call)
            {
                var call = RPCData.Call.Create(data);
                if (stubId == 0)
                    if (!localGlobalServiceIds.TryGetValue(call.GlobalServiceId, out stubId))
                        throw new Exception("Global service unavailable.");

                ((RPCImpl)stubs[stubId]).ProcessCall(call);
                return;
            }

            var stub = stubs[stubId];

            var impl = stub as RPCImpl;
            if (impl != null)
            {
                switch (data.Header.CallType)
                {
                    case RPCDataType.EventSubscribe:
                        impl.ProcessEventSubscribe(data);
                        return;
                    case RPCDataType.EventUnsubscribe:
                        impl.ProcessEventUnsubscribe(data);
                        return;
                    default:
                        throw new NotImplementedException();
                }
            }

            var proxy = (RPCProxy)stub;
            switch (data.Header.CallType)
            {
                case RPCDataType.Reply:
                    proxy.ProcessReply(RPCData.Reply.Create(data));
                    return;
                default:
                    throw new NotImplementedException();
            }
        }

        protected int GetNextStubId()
        {
            return Interlocked.Increment(ref stubIdCounter);
        }

        public T GetLocalService<T>(int implId)
        {
            var serviceAttribute = typeof (T).GetCustomAttribute<ServiceAttribute>(false);
            var globalServiceAttribute = typeof (T).GetCustomAttribute<GlobalServiceAttribute>(false);
            if (serviceAttribute == null && globalServiceAttribute == null)
                throw new Exception("Type has no attribute Service or GlobalService.");

            RPCStub stub;
            if(!stubs.TryGetValue(implId, out stub))
                throw new Exception("Local service implementation not found.");

            var impl = (RPCImpl)stub;
            return (T) impl.Impl;
        }

        public T GetRemoteService<T>(RPCPeer peer, int implId)
        {
            var serviceAttribute = typeof (T).GetCustomAttribute<ServiceAttribute>(false);
            var globalServiceAttribute = typeof (T).GetCustomAttribute<GlobalServiceAttribute>(false);
            if (serviceAttribute == null && globalServiceAttribute == null)
                throw new Exception("Type has no attribute Service or GlobalService.");

            return GetCreateProxy<T>(peer, implId);
        }

        public T GetGlobalLocalService<T>()
        {
            var globalServiceAttribute = typeof (T).GetCustomAttribute<GlobalServiceAttribute>(false);
            if (globalServiceAttribute == null)
                throw new Exception("Type has no attribute Service or GlobalService.");

            var globalServiceId = RPCManager.ContextManager.GetGlobalServiceId(typeof(T));

            int serviceId;
            if(!localGlobalServiceIds.TryGetValue(globalServiceId, out serviceId))
                    throw new Exception("Global service not available.");

            return GetLocalService<T>(serviceId);
        }

        public T GetGlobalRemoteService<T>(RPCPeer peer)
        {
            var globalServiceAttribute = typeof (T).GetCustomAttribute<GlobalServiceAttribute>(false);
            if (globalServiceAttribute == null)
                throw new Exception("Type has no attribute Service or GlobalService.");

            var globalServiceId = RPCManager.ContextManager.GetGlobalServiceId(typeof(T));
            var tuple = new Tuple<RPCPeer, GlobalServiceId>(peer, globalServiceId);
            RPCProxy globalServiceProxy;
            if(globalServiceProxies.TryGetValue(tuple, out globalServiceProxy))
                return (T) (object) globalServiceProxy;

            var ret = GetCreateProxy<T>(peer, 0);
            var proxy = (RPCProxy) (object) ret;

            globalServiceProxies.Add(tuple, proxy);
            stubIdToGlobalServiceId.Add(proxy.LocalId, globalServiceId);

            return ret;
        }

        public RPCImpl GetCreateImplementation<T>(T service)
        {
            var serviceAttribute = typeof (T).GetCustomAttribute<ServiceAttribute>(false);
            var globalServiceAttribute = typeof (T).GetCustomAttribute<GlobalServiceAttribute>(false);
            if (serviceAttribute == null && globalServiceAttribute == null)
                throw new Exception("Type has no attribute Service or GlobalService.");

            RPCImpl impl;
            if (impls.TryGetValue(service, out impl))
                return impl;

            var implId = GetNextStubId();

            var serviceType = typeof(T);

            var serviceAssembly = serviceType.Assembly;

            var stubsType = serviceAssembly.GetType(Helper.GetStubsClassName(serviceType, true));
            var implType = stubsType.GetNestedType("Impl");
            impl = (RPCImpl)Activator.CreateInstance(implType, service, implId);

            stubs.Add(implId, impl);
            impls.Add(service, impl);
            impl.RPCManager = RPCManager;

            if (globalServiceAttribute != null)
            {
                var globalServiceId = RPCManager.ContextManager.GetGlobalServiceId(typeof(T));
                localGlobalServiceIds.Add(globalServiceId, impl.LocalId);
                stubIdToGlobalServiceId.Add(implId, globalServiceId);
            }

            return impl;
        }

        private T GetCreateProxy<T>(RPCPeer peer, int implId)
        {
            var tuple = new Tuple<RPCPeer, int>(peer, implId);
            RPCProxy proxy;
            if (proxies.TryGetValue(tuple, out proxy))
                return (T)(object)proxy;

            var serviceType = typeof(T);
            var serviceAssembly = serviceType.Assembly;

            var implType = serviceAssembly.GetType(Helper.GetStubsClassName(serviceType, true));
            var proxyType = implType.GetNestedType("Proxy");

            var proxyId = GetNextStubId();

            var stub = Activator.CreateInstance(proxyType, peer, implId, proxyId);
            proxy = (RPCProxy) stub;

            stubs.Add(proxyId, proxy);
            proxies.Add(tuple, proxy);
            proxy.RPCManager = RPCManager;

            return (T)stub;
        }

        internal bool HasGlobalService(Type type)
        {
            var globalServiceId = RPCManager.ContextManager.GetGlobalServiceId(type);
            return localGlobalServiceIds.ContainsKey(globalServiceId);
        }

        internal GlobalServiceId GetGlobalServiceId(int stubId)
        {
            GlobalServiceId globalServiceId;
            if(stubIdToGlobalServiceId.TryGetValue(stubId, out globalServiceId))
                return globalServiceId;

            throw new Exception("Global service unavailable.");
        }
    }
}
