#pragma once

#if RK_REQUEST_HIGH_PERF_RENDERER
#    if SDS_OS_WINDOWS
#        include "core/platform/win32_include.h"

/**
 * \file request_high_perf_renderer.h
 * \brief Ask for a high performance graphics renderer.
 *
 * Both NVIDIA (NVIDIA Optimus) and AMD use dllexports to determine if a high
 * performance renderer should be used. This is primarily for systems with an
 * iGPU and dGPU like laptops where the system makes power optimizations.
 */

extern "C" {
/**  NVIDIA Optimus high performance request
 *
 * NVIDIA Optimus balances the iGPU and dGPU on systems. If you are not on an NVIDIA
 * application profile list, the game will not run on the GPU by default. By
 * setting this global export, it asks for a high performance renderer.
 *
 * TODO(sdsmith): It's unclear whether user application profile settings will take precedence over
 * this request.
 *
 * NOTE(sdsmith): Cannot be set at runtime! It is read when the app is getting loaded. Modifications
 * at runtime have no effect.
 *
 * ref:
https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
 */
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

/** AMD Switchable Graphics high performance request
 *
 * AMD Switchable Graphics balances the iGPU and dGPU on systems. By setting this global export, it
 * asks for a high performance renderer.
 *
 * Appears that setting this always overrides the application profile/power
 * settings. TODO(sdsmith): Unconfirmed.
 *
 * NOTE(sdsmith): Cannot be set at runtime! It is read when the app is getting loaded. Modifications
 * at runtime have no effect.
 *
 * ref: https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
 */
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#    elif SDS_OS_LINUX
#        // TODO(sdsmith): Can't figure out a quick way to control this from within the app. Live with it for now.
#    else
#        error Requesting high performance renderer not handled on this platform
#    endif
#endif
