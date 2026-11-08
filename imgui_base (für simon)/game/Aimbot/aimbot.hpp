#pragma once
#include "sdk/sdk.hpp"
#include "utils/memory.hpp"
#include "utils/settings/settings.hpp"

namespace fortnite {
    void ResetCamera(camera_manager* camera) {
        if (!camera) return;
        write<float>(uintptr_t(camera) + offsets::ViewPitchMin, -89.9999f);
        write<float>(uintptr_t(camera) + offsets::ViewPitchMax, 89.9999f);
        write<float>(uintptr_t(camera) + offsets::ViewYawMin, 0.0000f);
        write<float>(uintptr_t(camera) + offsets::ViewYawMax, 359.9999f);
    }

    void Camera_Aimbot(camera_manager* camera, fvector target_location)
    {
        if (!camera || !settings::aimbot::mouse) return;
        if (!target_location.x && !target_location.y && !target_location.z) return;

        fvector camera_location = camera->get_camera_location();
        frotator camera_rotation = camera->get_camera_rotation();

        frotator target_rotation = defines::math->find_look_at_rotation(camera_location, target_location);
        
        frotator delta = target_rotation - camera_rotation;
        
        if (delta.yaw > 180.0f) delta.yaw -= 360.0f;
        if (delta.yaw < -180.0f) delta.yaw += 360.0f;

        float smooth = settings::aimbot::mouse_speed;
        if (smooth < 1.0f) smooth = 1.0f;

        frotator smoothed_rotation;
        smoothed_rotation.pitch = camera_rotation.pitch + (delta.pitch / (double)smooth);
        smoothed_rotation.yaw = camera_rotation.yaw + (delta.yaw / (double)smooth);

        if (smoothed_rotation.yaw > 180.0f) smoothed_rotation.yaw -= 360.0f;
        if (smoothed_rotation.yaw < -180.0f) smoothed_rotation.yaw += 360.0f;

        write<float>(uintptr_t(camera) + offsets::ViewPitchMin, (float)smoothed_rotation.pitch);
        write<float>(uintptr_t(camera) + offsets::ViewPitchMax, (float)smoothed_rotation.pitch);
        write<float>(uintptr_t(camera) + offsets::ViewYawMin, (float)smoothed_rotation.yaw);
        write<float>(uintptr_t(camera) + offsets::ViewYawMax, (float)smoothed_rotation.yaw);
    }
}
