//
// Created by michiel on 24-1-2016.
//

#ifndef OPENPSTD_MOCKKERNEL_H
#define OPENPSTD_MOCKKERNEL_H

#include "KernelInterface.h"

class MockKernel: public KernelInterface
{
private:
    std::shared_ptr<PSTDFileConfiguration> _conf;
public:
    /**
     * Sets the configuration.
     */
    virtual void Configure(std::shared_ptr<PSTDFileConfiguration> config) override;

    /**
     * Runs the kernel. The callback has a single function that informs the rest of the
     * application of the progress of the kernel.
     * Must first be configured, else a PSTDKernelNotConfiguredException is thrown.
     */
    virtual void run(KernelCallback *callback) override;

    /**
     * Query the kernel for metadata about the simulation that is configured.
     */
    virtual SimulationMetadata GetSimulationMetadata() override;
};


#endif //OPENPSTD_MOCKKERNEL_H