/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2014-2020 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
/**
 ***********************************************************************************************************************
 * @file  palPerfExperiment.h
 * @brief Defines the Platform Abstraction Library (PAL) IPerfExperiment interface and related types.
 ***********************************************************************************************************************
 */

#pragma once

#include "pal.h"
#include "palGpuMemoryBindable.h"

namespace Pal
{

/// Specifies a particular block on the GPU to gather counters for.
enum class GpuBlock : uint32
{
    Cpf     = 0x0,
    Ia      = 0x1,
    Vgt     = 0x2,
    Pa      = 0x3,
    Sc      = 0x4,
    Spi     = 0x5,
    Sq      = 0x6,
    Sx      = 0x7,
    Ta      = 0x8,
    Td      = 0x9,
    Tcp     = 0xA,
    Tcc     = 0xB,
    Tca     = 0xC,
    Db      = 0xD,
    Cb      = 0xE,
    Gds     = 0xF,
    Srbm    = 0x10,
    Grbm    = 0x11,
    GrbmSe  = 0x12,
    Rlc     = 0x13,
    Dma     = 0x14,
    Mc      = 0x15,
    Cpg     = 0x16,
    Cpc     = 0x17,
    Wd      = 0x18,
    Tcs     = 0x19,
    Atc     = 0x1A,
    AtcL2   = 0x1B,
    McVmL2  = 0x1C,
    Ea      = 0x1D,
    Rpb     = 0x1E,
    Rmi     = 0x1F,
    Umcch   = 0x20,
    Ge      = 0x21,
    Gl1a    = 0x22,
    Gl1c    = 0x23,
    Gl1cg   = 0x24,
    Gl2a    = 0x25, // TCA is used in Gfx9, and changed to GL2A in Gfx10
    Gl2c    = 0x26, // TCC is used in Gfx9, and changed to GL2C in Gfx10
    Cha     = 0x27,
    Chc     = 0x28,
    Chcg    = 0x29,
    Gus     = 0x2A,
    Gcr     = 0x2B,
    Ph      = 0x2C,
#if PAL_CLIENT_INTERFACE_MAJOR_VERSION > 485
    UtcL1   = 0x2D,
#endif
    Count
};

/// Distinguishes between global and streaming performance monitor (SPM) counters.
enum class PerfCounterType : uint32
{
    Global = 0x0, ///< Represents the traditional summary perf counters.
    Spm    = 0x1, ///< Represents streaming performance counters.
    Count
};

/// Reports the type of data the hardware writes for a particular counter.
enum class PerfCounterDataType : uint32
{
    Uint32 = 0x0,
    Uint64 = 0x1,
    Count
};

/// Distinguishes between normal thread traces and streaming performance monitor (SPM) traces.
enum class PerfTraceType : uint32
{
    ThreadTrace = 0x0,
    SpmTrace    = 0x1,
    Count
};

/// Mask values ORed together to choose which shader stages a performance experiment should sample.
enum PerfExperimentShaderFlags
{
    PerfShaderMaskPs  = 0x01,
    PerfShaderMaskVs  = 0x02,
    PerfShaderMaskGs  = 0x04,
    PerfShaderMaskEs  = 0x08,
    PerfShaderMaskHs  = 0x10,
    PerfShaderMaskLs  = 0x20,
    PerfShaderMaskCs  = 0x40,
    PerfShaderMaskAll = 0x7f,
};

/// Selects one of two supported generic performance trace markers, which the client can use to track data of its own
/// choosing.
enum class PerfTraceMarkerType : uint32
{
    A = 0x0,
    B = 0x1,
    Count
};

/// Specifies available features in device for supporting performance measurements.
union PerfExperimentDeviceFeatureFlags
{
    struct
    {
        uint32 counters          :  1; ///< Device supports performance counters.
        uint32 threadTrace       :  1; ///< Device supports thread traces.
        uint32 spmTrace          :  1; ///< Device supports streaming perf monitor traces.
        uint32 supportPs1Events  :  1; ///< The thread trace HW of this Device is capable of producing event tokens
                                       ///  from the second PS backend of SC.
        uint32 sqttBadScPackerId :  1; ///< Hardware is affected by bug causing the packer ID specified in new PS waves
                                       ///  to be incorrect in SQ thread trace data.
        uint32 reserved          : 26; ///< Reserved for future use.
    };
    uint32     u32All;                 ///< Feature flags packed as 32-bit uint.
};

/// Specifies properties for a perf counter being added to a perf experiment.  Input structure to
/// IPerfExperiment::AddCounter().
struct PerfCounterInfo
{
    PerfCounterType              counterType; ///< Type of counter to add.
    GpuBlock                     block;       ///< Which block to reference.
    uint32                       instance;    ///< Instance of that block in the device.
    uint32                       eventId;     ///< Which event ID to track.

#if   PAL_CLIENT_INTERFACE_MAJOR_VERSION < 543
    union
    {
        struct
        {
            // SQ counter specific parameters
            uint32 sqSimdMask      :  1;

            // SQ counter specific parameters for GFXIP7+
            uint32 sqSqcBankMask   :  1;
            uint32 sqSqcClientMask :  1;

            uint32 reserved        : 29;
        };
        uint32 u32All;
    } optionFlags;

    struct
    {
        // SQ counter specific parameters
        uint32 sqSimdMask;

        // SQ counter specific parameters for GFXIP7+
        uint32 sqSqcBankMask;
        uint32 sqSqcClientMask;
    } optionValues;
#endif
};

/// Specifies properties for setting up a streaming performance counter trace. Input structure to
/// IPerfExperiment::AddSpmTrace().
struct SpmTraceCreateInfo
{
    uint32                 spmInterval;       ///< Interval between each sample in terms of GPU sclks. Minimum of 32.
    gpusize                ringSize;          ///< Size of the SPM output ring buffer in bytes.
    uint32                 numPerfCounters;   ///< Number of performance counters to be collected in this trace.
    const PerfCounterInfo* pPerfCounterInfos; ///< Array of size numPerfCounters of PerfCounterInfo(s).
};

/// Reports layout of a single global perf counter sample.
struct GlobalSampleLayout
{
    GpuBlock            block;             ///< Type of GPU block.
    uint32              instance;          ///< Which instance of that type of GPU block.
    uint32              slot;              ///< Slot varies in meaning per block.
    uint32              eventId;           ///< Sampled event ID.
    PerfCounterDataType dataType;          ///< What type of data is written (e.g., 32-bit uint).
    gpusize             beginValueOffset;  ///< Offset in bytes where the sample data begins.
    gpusize             endValueOffset;    ///< Offset in bytes where the sample data ends.
};

/// Describes the layout of global perf counter data in memory.
struct GlobalCounterLayout
{
    uint32             sampleCount;  ///< Number of samples described in samples[].
    GlobalSampleLayout samples[1];   ///< Describes the layout of each sample.  This structure is repeated (sampleCount
                                     ///  - 1) additional times.
};

/// Enumeration of SQ Thread trace token types. All versions of Thread Trace (TT) are represented. If an unsupported
/// token is enabled, no error is reported.
enum ThreadTraceTokenTypeFlags : Pal::uint32
{
    Misc         = 0x00000001, ///< A miscellaneous event has been sent. TT 2.3
    Timestamp    = 0x00000002, ///< Timestamp tokens. TT 2.3
    Reg          = 0x00000004, ///< Register activity token. TT 2.3
    WaveStart    = 0x00000008, ///< A wavefront has started. TT 2.3
    WaveAlloc    = 0x00000010, ///< Output space has been allocated for vertex position or color/Z. TT 2.3.
    RegCsPriv    = 0x00000020, ///< There has been a compute pipeline private data, state or threadgroup update. TT 2.3.
    WaveEnd      = 0x00000040, ///< Wavefront completion. TT 2.3
    Event        = 0x00000080, ///< An event has reached the top of a shader stage. TT 2.3
    EventCs      = 0x00000100, ///< An event has reached the top of a compute shader stage. TT 2.3
    EventGfx1    = 0x00000200, ///< An event has reached the top of a shader stage for the second GFX pipe. TT 2.3
    Inst         = 0x00000400, ///< The shader has executed an instruction. TT 2.3
    InstPc       = 0x00000800, ///< The shader has explicitly written the PC value. TT 2.3
    InstUserData = 0x00001000, ///< The shader has written user data into the thread trace buffer. TT 2.3
    Issue        = 0x00002000, ///< Provides information about instruction scheduling. TT 2.3
    Perf         = 0x00004000, ///< The performance counter delta has been updated. TT 2.3 and below only.
    RegCs        = 0x00008000, ///< A compute  state update packet has been received by the SPI. TT 2.3
    VmemExec     = 0x00010000, ///< A previously issued VMEM instruction is now being sent to LDS/TA. TT 3.0
    AluExec      = 0x00020000, ///< A previously issued VALU instruction is now being executed. TT 3.0
    ValuInst     = 0x00040000, ///< A VALU instruction has been issued. TT 3.0.
    WaveRdy      = 0x00080000, ///< Mask of which waves became ready this cycle but did not issue an instruction. TT 3.0
    Immed1       = 0x00100000, ///< One wave issued an immediate instruction this cycle. TT 3.0.
    Immediate    = 0x00200000, ///< One or more waves have issued an immediate instruction this cycle. TT 3.0.
    UtilCounter  = 0x00400000, ///< A new set of utilization counter values. TT 3.0.
    All          = 0xFFFFFFFF  ///< Enable all the above tokens.
};

/// Enumeration of register types whose reads/writes can be traced. Register reads are disabled by default as it can
/// generate a lot of traffic and cause the GPU to hang.
enum ThreadTraceRegTypeFlags : Pal::uint32
{
    EventRegs             = 0x00000001, ///< Event registers. TT 2.3.
    DrawRegs              = 0x00000002, ///< Draw registers. TT 2.3.
    DispatchRegs          = 0x00000004, ///< Dispatch registers. TT 2.3.
    UserdataRegs          = 0x00000008, ///< UserData Registers. Must be explicitly requested in TT 2.3.
    MarkerRegs            = 0x00000010, ///< Thread trace marker data regs. TT 2.3.
    ShaderConfigRegs      = 0x00000020, ///< Shader configuration state. TT 3.0.
    ShaderLaunchStateRegs = 0x00000040, ///< Shader program launch state. TT 3.0.
    GraphicsPipeStateRegs = 0x00000080, ///< Graphics pipeline state. TT 3.0.
    AsyncComputeRegs      = 0x00000100, ///< Async compute registers. TT 3.0.
    GraphicsContextRegs   = 0x00000200, ///< Graphics context registers. TT 3.0.
    OtherConfigRegs       = 0x00000400, ///< Other regs. TT 2.3.
#if PAL_CLIENT_INTERFACE_MAJOR_VERSION >= 544
    AllRegWrites          = 0x000007FF, ///< All reg writes other than OtherBusRegs.
    OtherBusRegs          = 0x00000800, ///< All write activity over gfx and compute buses. Debug only. TT 3.0.
    AllRegReads           = 0x00001000, ///< Not encouraged to be enabled. This can cause a GPU hang.
#else
    OtherBusRegs          = 0x00000800, ///< All write activity over gfx and compute buses. Debug only. TT 3.0.
    AllRegWrites          = 0x00001FFF, ///< All reg writes other than OtherBusRegs.
    AllRegReads           = 0x00002000, ///< Not encouraged to be enabled. This can cause a GPU hang.
#endif
    AllReadsAndWrites     = 0xFFFFFFFF  ///< All reads and writes. Not encouraged. This can cause a GPU hang.
};

/// Represents thread trace token types and register types that can be enabled to be reported in the trace data. If
/// a particular token type or reg type is unsupported, no error is returned and the thread trace is configured with
/// the minimum supported tokens in the user provided config.
struct ThreadTraceTokenConfig
{
    /// Mask of ThreadTraceTokenTypeFlags
    uint32 tokenMask;

    /// Mask of ThreadTraceRegTypeFlags
    uint32 regMask;
};

/// Specifies properties for a perf trace being added to a perf experiment.  Input structure to
/// IPerfExperiment::AddThreadTrace().
struct ThreadTraceInfo
{
    PerfTraceType              traceType;    ///< Type of trace to add.
    uint32                     instance;     ///< Selected trace instance.

    union
    {
        struct
        {
            // Options common to all traces
            uint32 bufferSize                :  1;

            // Thread trace only options
            uint32 threadTraceTargetSh       :  1;
            uint32 threadTraceTargetCu       :  1;
            uint32 threadTraceSh0CounterMask :  1;
            uint32 threadTraceSh1CounterMask :  1;
            uint32 threadTraceSimdMask       :  1;
            uint32 threadTraceVmIdMask       :  1;
            uint32 threadTraceRandomSeed     :  1;
            uint32 threadTraceShaderTypeMask :  1;
            uint32 threadTraceIssueMask      :  1;
            uint32 threadTraceWrapBuffer     :  1;
            uint32 threadTraceStallBehavior  :  1;
            uint32 threadTraceTokenConfig    :  1;
            uint32 reserved                  : 19;
        };
        uint32 u32All;
    } optionFlags;

    struct
    {
        // Options common to all traces
        size_t                    bufferSize;

        // Thread trace only options
        ThreadTraceTokenConfig    threadTraceTokenConfig;
        uint32                    threadTraceTargetSh;
        uint32                    threadTraceTargetCu;
        uint32                    threadTraceSh0CounterMask;
        uint32                    threadTraceSh1CounterMask;
        uint32                    threadTraceSimdMask;
        uint32                    threadTraceVmIdMask;
        uint32                    threadTraceRandomSeed;
        PerfExperimentShaderFlags threadTraceShaderTypeMask;
        uint32                    threadTraceIssueMask;
        bool                      threadTraceWrapBuffer;
        uint32                    threadTraceStallBehavior;
    } optionValues;
};

/// Reports thread trace data written when the trace is stopped (copied from internal SQ registers).
struct ThreadTraceInfoData
{
    uint32 curOffset;     ///< Contents of SQ_THREAD_TRACE_WPTR register.
    uint32 traceStatus;   ///< Contents of SQ_THREAD_TRACE_STATUS register.
    uint32 writeCounter;  ///< Contents of SQ_THREAD_TRACE_CNTR register.
};

/// Describes the layout of a single shader engine's thread trace data.
struct ThreadTraceSeLayout
{
    uint32  shaderEngine;  ///< Shader engine index.
    uint32  computeUnit;   ///< Compute unit index.
    gpusize infoOffset;    ///< Offset to ThreadTraceInfoData in memory.
    gpusize infoSize;      ///< Size in bytes reserved for ThreadTraceInfoData.
    gpusize dataOffset;    ///< Offset in bytes to the actual trace data.
    gpusize dataSize;      ///< Amount of trace data, in bytes.
};

/// Describes how the thread trace data is laid out.
struct ThreadTraceLayout
{
    uint32              traceCount;  ///< Number of entries in traces[].
    ThreadTraceSeLayout traces[1];   ///< ThreadTraceSeLayout repeated (traceCount - 1) times.
};

/// Represents all the segments in the spm trace sample. Global segment contains all the counter data for the blocks
/// that are outside the shader engines.
enum class SpmDataSegmentType : uint32
{
    Se0,
    Se1,
    Se2,
    Se3,
    Global,
    Count
};

/// Represents all data pertaining to a single spm counter instance.
struct SpmCounterData
{
    SpmDataSegmentType segment;  ///< Segment this counter belongs to (global, Se0, Se1 etc).
    gpusize            offset;   ///< Offset within the segment where the counter data lies.
    GpuBlock           gpuBlock; ///< The gpu block this counter instance belongs to.
    uint32             instance; ///< The global instance number of this counter.
    uint32             eventId;  ///< The event that was tracked by this counter.
};

/// Represents all information required for reading contents of SpmTrace results buffer. The caller must provide
/// enough memory for storing (numCounters-1) * sizeof(SpmCounterData), following this structure.
struct SpmTraceLayout
{
    gpusize        offset;            ///< Offset into the buffer where the spm trace data begins.
    gpusize        wptrOffset;        ///< Offset of the dword that has the size of spm data written by the HW.
    gpusize        sampleOffset;      ///< Offset into the buffer where the first sample data begins.
    uint32         sampleSizeInBytes; ///< Size of all segments in one sample.
    uint32         segmentSizeInBytes[static_cast<uint32>(SpmDataSegmentType::Count)]; ///< Individual segment sizes.
    uint32         numCounters;       ///< Number of counters for which spm trace was requested by the client.
    SpmCounterData counterData[1];    ///< Contains numCounters - 1 CounterInfo
};

/// Specifies properties for creation of an @ref IPerfExperiment object.  Input structure to
/// IDevice::CreatePerfExperiment().
struct PerfExperimentCreateInfo
{
    union
    {
        struct
        {
            uint32 cacheFlushOnCounterCollection :  1;
            uint32 sampleInternalOperations      :  1;
            uint32 sqShaderMask                  :  1;
            uint32 reserved                      : 29;
        };
        uint32 u32All;
    } optionFlags;

    struct
    {
        bool                      cacheFlushOnCounterCollection;
        bool                      sampleInternalOperations;
        PerfExperimentShaderFlags sqShaderMask;
    } optionValues;
};

/**
 ***********************************************************************************************************************
 * @interface IPerfExperiment
 * @brief     Set of performance profiling activities to be performed over a specific range of commands in a command
 *            buffer.
 *
 * @warning The details of building a performance experiment are not very well documented here.  Please see your local
 *          hardware performance expert for more details until this documentation can be fully fleshed out.
 *
 * @see IDevice::CreatePerfExperiment
 ***********************************************************************************************************************
 */
class IPerfExperiment : public IGpuMemoryBindable
{
public:
    /// Adds the specified performance counter to be tracked as part of this perf experiment.
    ///
    /// @param [in] counterInfo Specifies which counter to add: which hardware block, instance, any options, etc.
    ///
    /// @returns Success if the counter was successfully added to the experiment, otherwise an appropriate error code.
    virtual Result AddCounter(
        const PerfCounterInfo& counterInfo) = 0;

    /// Queries the layout of counter results in memory for this perf experiment.
    ///
    /// @param [out] pLayout Layout describing the begin and end offset of each counter in the resulting GPU memory once
    ///                      this perf experiment is executed.  Should correspond with counters added via AddCounter().
    ///
    /// @returns Success if the layout was successfully returned in pLayout, otherwise an appropriate error code.
    virtual Result GetGlobalCounterLayout(
        GlobalCounterLayout* pLayout) const = 0;

    /// Addes the specified thread trace to be recorded as part of this perf experiment.
    ///
    /// @param [in] traceInfo Specifies what type of trace to record, which block instance to trace, and options, etc.
    ///
    /// @returns Success if the trace was successfully added to the experiment, otherwise an appropriate error code.
    virtual Result AddThreadTrace(
        const ThreadTraceInfo& traceInfo) = 0;

    /// Adds the specified SpmTrace to be recorded as part of this perf experiment.
    ///
    /// @param [in] spmCreateInfo Specifies the parameters of the spm trace and provides the list of perf counters.
    ///
    /// @returns Success if the spm trace was successfully added to the experiment, otherwise an appropriate error code.
    virtual Result AddSpmTrace(
        const SpmTraceCreateInfo& spmCreateInfo) = 0;

    /// Queries the layout of thread trace results in memory for this perf experiment.
    ///
    /// @param [out] pLayout Layout describing how the results of each thread trace will be written to GPU memory when
    ///                      this perf experiment is executed.  Should correspond with counters added via AddTrace().
    ///
    /// @returns Success if the layout was successfully returned in pLayout, otherwise an appropriate error code.
    virtual Result GetThreadTraceLayout(
        ThreadTraceLayout* pLayout) const = 0;

    /// Queries the layout of streaming counter trace results in memory for this perf experiment.
    ///
    /// @param [out] pLayout Layout describing the layout of the streaming counter trace results in the resulting
    ///                      GPU memory once this perf experiment is executed.
    ///
    /// @returns Success if the layout was successfully returned in pLayout, otherwise an appropriate error code.
    virtual Result GetSpmTraceLayout(
        SpmTraceLayout* pLayout) const = 0;

    /// Finalizes the performance experiment preparing it for execution.
    ///
    /// @returns Success if the operation executed successfully, otherwise an appropriate error code.
    virtual Result Finalize() = 0;

    /// Returns the value of the associated arbitrary client data pointer.
    /// Can be used to associate arbitrary data with a particular PAL object.
    ///
    /// @returns Pointer to client data.
    PAL_INLINE void* GetClientData() const
    {
        return m_pClientData;
    }

    /// Sets the value of the associated arbitrary client data pointer.
    /// Can be used to associate arbitrary data with a particular PAL object.
    ///
    /// @param  [in]    pClientData     A pointer to arbitrary client data.
    PAL_INLINE void SetClientData(
        void* pClientData)
    {
        m_pClientData = pClientData;
    }

protected:
    /// @internal Constructor. Prevent use of new operator on this interface. Client must create objects by explicitly
    /// called the proper create method.
    IPerfExperiment() : m_pClientData(nullptr) {}

    /// @internal Destructor.  Prevent use of delete operator on this interface.  Client must destroy objects by
    /// explicitly calling IDestroyable::Destroy() and is responsible for freeing the system memory allocated for the
    /// object on their own.
    virtual ~IPerfExperiment() { }

private:
    /// @internal Client data pointer. This can have an arbitrary value and can be returned by calling GetClientData()
    /// and set via SetClientData().
    /// For non-top-layer objects, this will point to the layer above the current object.
    void* m_pClientData;
};

} // Pal
