//
// Created by Bartek on 07.05.2024.
//

#ifndef EVILENGINE_EDITOR_HPP
#define EVILENGINE_EDITOR_HPP

#endif //EVILENGINE_EDITOR_HPP


namespace EDITOR {

    const int PLAY_MODE = 0;
    const int EDIT_MODE = 1;

    void EditTransform(glm::mat4 &matrix, glm::mat4 &view, glm::mat4 &projection)
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
//        float model[16];
//        for (int i = 0; i < 16; i++)
//        {
//            model[i] = matrix[0][1];
//        }
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(matrix), matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(matrix));

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
//        ImGui::Checkbox("", &useSnap);
//        ImGui::SameLine();
//        vec_t snap;
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
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(matrix));
        //ImGuizmo::Manipulate(camera.mView.m16, camera.mProjection.m16, mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m16, NULL, useSnap ? &snap.x : NULL);
    }
};