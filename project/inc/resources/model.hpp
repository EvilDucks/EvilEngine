#pragma once

#include "../render/mesh.hpp"
#include "../render/material.hpp"
#include "json.hpp"

namespace MODEL {

    struct Model {
//        u8 materialsCount;
//        MATERIAL::Material* materials;
//        u8 meshesCount;
//        MESH::Mesh* meshes;

        // Variables for easy access
        const char* file;
        std::vector<unsigned char> data;
        RESOURCES::Json JSON;

        // All the meshes and transformations
        std::vector<MESH::Mesh> meshes;
        std::vector<glm::vec3> translationsMeshes;
        std::vector<glm::quat> rotationsMeshes;
        std::vector<glm::vec3> scalesMeshes;
        std::vector<glm::mat4> matricesMeshes;

        // Prevents textures from being loaded twice
        std::vector<std::string> loadedTexName;
        //std::vector<Texture> loadedTex;
    };

    std::string Get_file_contents(const char* filename);

    void Create(MODEL::Model& model, const char* file);
    //void Draw(Shader& shader, Camera& camera);

    // Loads a single mesh by its index
    void LoadMesh(MODEL::Model& model, unsigned int indMesh);

    // Traverses a node recursively, so it essentially traverses all connected nodes
    void TraverseNode(MODEL::Model& model, unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

    // Gets the binary data from a file
    std::vector<unsigned char> GetData(MODEL::Model& model);

    // Interprets the binary data into floats, indices, and textures
    std::vector<float> GetFloats(MODEL::Model& model, RESOURCES::Json accessor);
    std::vector<GLuint> GetIndices(MODEL::Model& model, RESOURCES::Json accessor);
    //std::vector<Texture> GetTextures();

    // Assembles all the floats into vertices
    std::vector<glm::vec3> AssembleVertices
            (
                    MODEL::Model& model,
                    std::vector<glm::vec3> positions,
                    std::vector<glm::vec3> normals,
                    std::vector<glm::vec2> texUVs
            );

    // Helps with the assembly from above by grouping floats
    std::vector<glm::vec2> GroupFloatsVec2(MODEL::Model& model, std::vector<float> floatVec);
    std::vector<glm::vec3> GroupFloatsVec3(MODEL::Model& model, std::vector<float> floatVec);
    std::vector<glm::vec4> GroupFloatsVec4(MODEL::Model& model, std::vector<float> floatVec);

    std::string Get_file_contents(const char* filename)
    {
//        std::ifstream in(filename, std::ios::binary);
//        if (in)
//        {
//            std::string contents;
//            in.seekg(0, std::ios::end);
//            contents.resize(in.tellg());
//            in.seekg(0, std::ios::beg);
//            in.read(&contents[0], contents.size());
//            in.close();
//            return(contents);
//        }
//        throw(errno);
    }

    void Create(MODEL::Model& model, const char* file)
    {
        // Make a JSON object
        //std::string text = Get_file_contents(file);
        //model.JSON = RESOURCES::Json::parse(text);

        // Get the binary data
        //model.file = file;
        //model.data = MODEL::GetData(model);


        int x = 0;
        // Traverse all nodes
        //MODEL::TraverseNode(model, 0);
    }

    std::vector<unsigned char> GetData(MODEL::Model& model)
    {
        // Create a place to store the raw text, and get the uri of the .bin file
        std::string bytesText;
        std::string uri = model.JSON["buffers"][0]["uri"];

        // Store raw text data into bytesText
        std::string fileStr = std::string(model.file);
        std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
        bytesText = Get_file_contents((fileDirectory + uri).c_str());

        // Transform the raw text data into bytes and put them in a vector
        std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
        return data;
    }

}
