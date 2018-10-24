﻿//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//
#pragma once

#include <armnn/Tensor.hpp>
#include <armnn/Types.hpp>

#include "Network.hpp"
#include "LayerFwd.hpp"
#include "Profiling.hpp"

#include <backends/IBackendInternal.hpp>
#include <backends/Workload.hpp>
#include <backends/WorkloadFactory.hpp>

#include <mutex>
#include <unordered_map>

namespace cl
{
    class Context;
    class CommandQueue;
    class Device;
}

namespace armnn
{

class LoadedNetwork
{
public:
    using WorkloadQueue = std::vector< std::unique_ptr<IWorkload> >;
    ~LoadedNetwork(){ FreeWorkingMemory(); }

    TensorInfo GetInputTensorInfo(LayerBindingId layerId) const;
    TensorInfo GetOutputTensorInfo(LayerBindingId layerId) const;

    Status EnqueueWorkload(const InputTensors& inputTensors, const OutputTensors& outputTensors);

    static std::unique_ptr<LoadedNetwork> MakeLoadedNetwork(std::unique_ptr<OptimizedNetwork> net,
                                                            const IRuntime::CreationOptions& options,
                                                            std::string & errorMessage);

    // NOTE we return by reference as the purpose of this method is only to provide
    // access to the private m_Profiler and in theory we should not need to increment
    // the shared_ptr's reference counter
    const std::shared_ptr<Profiler>& GetProfiler() const { return m_Profiler; }

    void AllocateWorkingMemory();
    void FreeWorkingMemory();

private:
    LoadedNetwork(std::unique_ptr<OptimizedNetwork> net, const IRuntime::CreationOptions& options);

    void EnqueueInput(const BindableLayer& layer, ITensorHandle* tensorHandle, const TensorInfo& tensorInfo);

    void EnqueueOutput(const BindableLayer& layer, ITensorHandle* tensorHandle, const TensorInfo& tensorInfo);

    bool Execute();

    const IWorkloadFactory& GetWorkloadFactory(const Layer& layer) const;

    using BackendPtrMap = std::unordered_map<BackendId, IBackendInternalUniquePtr>;
    using WorkloadFactoryMap = std::unordered_map<BackendId, IBackendInternal::IWorkloadFactoryPtr>;

    BackendPtrMap       m_Backends;
    WorkloadFactoryMap  m_WorkloadFactories;

    std::unique_ptr<OptimizedNetwork> m_OptimizedNetwork;
    WorkloadQueue m_InputQueue;
    WorkloadQueue m_WorkloadQueue;
    WorkloadQueue m_OutputQueue;
    std::shared_ptr<Profiler> m_Profiler;

    using UniqueMutexLock = std::unique_lock<std::mutex>;
    mutable std::mutex m_WorkingMemMutex;
    UniqueMutexLock m_WorkingMemLock;

    bool m_IsWorkingMemAllocated=false;
};

}
