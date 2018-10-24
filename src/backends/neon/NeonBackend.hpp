//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//
#pragma once

#include <backends/IBackendInternal.hpp>

namespace armnn
{

class NeonBackend : public IBackendInternal
{
public:
    NeonBackend() = default;
    ~NeonBackend() override = default;

    static const BackendId& GetIdStatic();
    const BackendId& GetId() const override { return GetIdStatic(); }

    IWorkloadFactoryPtr CreateWorkloadFactory() const override;
};

} // namespace armnn