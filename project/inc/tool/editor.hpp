//
// Created by Bartek on 07.05.2024.
//

#ifndef EVILENGINE_EDITOR_HPP
#define EVILENGINE_EDITOR_HPP

#endif //EVILENGINE_EDITOR_HPP

#include "../components/transform.hpp"

namespace EDITOR {

    const int PLAY_MODE = 0;
    const int EDIT_MODE = 1;

    int currentSelection = 6;
    glm::vec3 selectionPosition;
    bool editLock = false;
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    struct Config {
        glm::vec3 mSnapTranslation = glm::vec3(1.f);
        glm::vec3 mSnapRotation = glm::vec3(1.f);
        glm::vec3 mSnapScale = glm::vec3(1.f);
    };

    Config config;

    void EditTransform(glm::vec3 &position, glm::vec3 &rotation, glm::vec3 &scale, glm::mat4 &view, glm::mat4 &projection)
    {
        if (ImGui::IsKeyPressed(71))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        glm::mat4 model;
        //ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));

        matrixTranslation[0] = position.x;
        matrixTranslation[1] = position.y;
        matrixTranslation[2] = position.z;
        matrixRotation[0] = rotation.x;
        matrixRotation[1] = rotation.y;
        matrixRotation[2] = rotation.z;
        matrixScale[0] = scale.x;
        matrixScale[1] = scale.y;
        matrixScale[2] = scale.z;

        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);

        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale), glm::value_ptr(model));

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
//        static bool useSnap(false);
//        if (ImGui::IsKeyPressed(83))
//            useSnap = !useSnap;
//        ImGui::Checkbox("Use snap", &useSnap);
//        ImGui::SameLine();
//        glm::vec3 snap;
//        switch (mCurrentGizmoOperation)
//        {
//            case ImGuizmo::TRANSLATE:
//                snap = config.mSnapTranslation;
//                ImGui::InputFloat3("Snap", &snap.x);
//                break;
//            case ImGuizmo::ROTATE:
//                snap = config.mSnapRotation;
//                ImGui::InputFloat("Angle Snap", &snap.x);
//                break;
//            case ImGuizmo::SCALE:
//                snap = config.mSnapScale;
//                ImGui::InputFloat("Scale Snap", &snap.x);
//                break;
//        }

        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(model));
        if (!editLock)
        {
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));
        }
    }

    void SelectObject(TRANSFORM::LTransform* transforms, u64 transformsCount, glm::vec3 position)
    {
        float minDistance = sqrt(pow((transforms[0].local.position.x - position.x), 2) + pow((transforms[0].local.position.y - position.y), 2) + pow((transforms[0].local.position.z - position.z), 2));
        int index = 0;

        float distance;
        for (int i = 1; i < transformsCount; i++)
        {
            distance = sqrt(pow((transforms[i].local.position.x - position.x), 2) + pow((transforms[i].local.position.y - position.y), 2) + pow((transforms[i].local.position.z - position.z), 2));
            if (distance < minDistance)
            {
                minDistance = distance;
                index = i;
            }
        }
        currentSelection = index;
    }

    void ShowGizmos(TRANSFORM::LTransform* transforms, u64 transformsCount, glm::mat4 &view, glm::mat4 &projection)
    {
        if (ImGui::IsKeyPressed(90))
        {
            editLock = !editLock;
            spdlog::info ("Edit lock: {0}", editLock);
        }

        ImGui::InputInt("Current selection index", &currentSelection);

        if (currentSelection > transformsCount - 1)
        {
            int test = currentSelection;
            currentSelection = transformsCount - 1;
            int test2 = currentSelection;
        }

        glm::mat4 model;

        glm::mat4 selectionModel = glm::mat4(1.f);
        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(selectionPosition), glm::value_ptr(glm::vec3(0.f)), glm::value_ptr(glm::vec3(1.f)), glm::value_ptr(selectionModel));
        //ImGuizmo::DrawCubes(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(selectionModel), 1);
        if (ImGui::IsMouseClicked(0))
        {
            ImGuizmo::GetTranslationPlanOrigin(glm::value_ptr(selectionPosition));
            SelectObject(transforms, transformsCount, selectionPosition);
            spdlog::info ("Current selection: {0}", currentSelection);
        }

        //ImGuizmo::Enable(false);
        for (int i = 0; i < transformsCount; i++)
        {
            ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(transforms[i].local.position), glm::value_ptr(transforms[i].local.rotation), glm::value_ptr(transforms[i].local.scale), glm::value_ptr(model));
            if (i != currentSelection)
            {
                ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(model));
            }
            else
            {
                //ImGuizmo::Enable(true);
                EditTransform(transforms[i].local.position, transforms[i].local.rotation, transforms[i].local.scale, view, projection);
                transforms[currentSelection].flags = 1;
                //ImGuizmo::Enable(false);
            }
        }
    }
};