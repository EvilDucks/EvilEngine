//
// Created by Bartek on 07.05.2024.
//

#ifndef EVILENGINE_EDITOR_HPP
#define EVILENGINE_EDITOR_HPP

#endif //EVILENGINE_EDITOR_HPP


namespace EDITOR {

    const int PLAY_MODE = 0;
    const int EDIT_MODE = 1;

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
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
        if (ImGui::IsKeyPressed(90))
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

        //ImGuizmo::DrawCubes(glm::value_ptr(view), glm::value_ptr(projection), glm::value_ptr(matrix), 1);
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(model));
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), glm::value_ptr(position), glm::value_ptr(rotation), glm::value_ptr(scale));
        //ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(matrix), NULL, useSnap ? &snap.x : NULL);
    }
};