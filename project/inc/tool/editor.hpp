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
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    struct Config {
        glm::vec3 mSnapTranslation = glm::vec3(1.f);
        glm::vec3 mSnapRotation = glm::vec3(1.f);
        glm::vec3 mSnapScale = glm::vec3(1.f);
    };

    Config config;

    void ApplyModel (glm::mat4& model, glm::vec3 &position, glm::vec3 &rotation, glm::vec3 &scale) {
        model = glm::translate (model, position);
        model = glm::scale (model, scale);
        model = glm::rotate (model, glm::radians (rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
        model = glm::rotate (model, glm::radians (rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
        model = glm::rotate (model, glm::radians (rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));
    }

    void EditTransform(glm::vec3 &position, glm::vec3 &rotation, glm::vec3 &scale, glm::mat4 &view, glm::mat4 &projection)
    {



        if (ImGui::IsKeyPressed(71))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(82))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(83)) // r Key
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

        position.x = matrixTranslation[0];
        position.y = matrixTranslation[1];
        position.z = matrixTranslation[2];
        rotation.x = matrixRotation[0];
        rotation.y = matrixRotation[1];
        rotation.z = matrixRotation[2];
        scale.x = matrixScale[0];
        scale.y = matrixScale[1];
        scale.z = matrixScale[2];

        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale), glm::value_ptr(model));

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }

        //ImGuizmo::DrawCubes(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(matrix), 1);
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(model));
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));

//        glm::mat4 test = glm::mat4(1.f);
//        ImGuizmo::Enable(false);
//        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(test));
//        ImGuizmo::Enable(true);

        if (ImGuizmo::IsOver() && ImGui::IsMouseClicked(0))
            ImGuizmo::Enable(true);
        if (ImGui::IsKeyPressed(74))
            ImGuizmo::Enable(false);
    }

    void ShowGizmos(TRANSFORM::LTransform* transforms, u64 transformsCount, glm::mat4 &view, glm::mat4 &projection)
    {
        glm::mat4 model;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
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
                //ImGuizmo::Enable(false);
            }

        }
    }
};