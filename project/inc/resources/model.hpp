#pragma once

#include <fstream>
#include "../render/mesh.hpp"
#include "../render/material.hpp"
#include "json.hpp"

namespace MODEL {

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 texUVs;
    };


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
    std::vector<Vertex> AssembleVertices
            (
                    std::vector<glm::vec3> positions,
                    std::vector<glm::vec3> normals,
                    std::vector<glm::vec2> texUVs
            );

    // Helps with the assembly from above by grouping floats
    std::vector<glm::vec2> GroupFloatsVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> GroupFloatsVec3(std::vector<float> floatVec);
    std::vector<glm::vec4> GroupFloatsVec4(std::vector<float> floatVec);

    std::string Get_file_contents(const char* filename)
    {
        std::ifstream in(filename, std::ios::binary);
        if (in)
        {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return(contents);
        }
        throw(errno);
    }

    void Create(MODEL::Model& model, const char* file)
    {
        // Make a JSON object
        std::string text = Get_file_contents(file);
        model.JSON = RESOURCES::Json::parse(text);

        // Get the binary data
        model.file = file;
        model.data = MODEL::GetData(model);



        // Traverse all nodes
        //MODEL::TraverseNode(model, 0);
    }

    std::vector<unsigned char> GetData(MODEL::Model& model)
    {
        try {
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
        } catch (const std::exception& e) {
            // Handle any exceptions (e.g., file not found, etc.)
            std::cerr << "Error in GetData: " << e.what() << std::endl;
            // You might want to return an empty vector or throw an exception here
            throw; // Re-throw the exception for the caller to handle
        }
    }

    void TraverseNode(MODEL::Model& model, unsigned int nextNode, glm::mat4 matrix)
    {
        // Current node
        RESOURCES::Json node = model.JSON["nodes"][nextNode];

        // Get translation if it exists
        glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
        if (node.find("translation") != node.end())
        {
            float transValues[3];
            for (unsigned int i = 0; i < node["translation"].size(); i++)
                transValues[i] = (node["translation"][i]);
            translation = glm::make_vec3(transValues);
        }
        // Get quaternion if it exists
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        if (node.find("rotation") != node.end())
        {
            float rotValues[4] =
                    {
                            node["rotation"][3],
                            node["rotation"][0],
                            node["rotation"][1],
                            node["rotation"][2]
                    };
            rotation = glm::make_quat(rotValues);
        }
        // Get scale if it exists
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        if (node.find("scale") != node.end())
        {
            float scaleValues[3];
            for (unsigned int i = 0; i < node["scale"].size(); i++)
                scaleValues[i] = (node["scale"][i]);
            scale = glm::make_vec3(scaleValues);
        }
        // Get matrix if it exists
        glm::mat4 matNode = glm::mat4(1.0f);
        if (node.find("matrix") != node.end())
        {
            float matValues[16];
            for (unsigned int i = 0; i < node["matrix"].size(); i++)
                matValues[i] = (node["matrix"][i]);
            matNode = glm::make_mat4(matValues);
        }

        // Initialize matrices
        glm::mat4 trans = glm::mat4(1.0f);
        glm::mat4 rot = glm::mat4(1.0f);
        glm::mat4 sca = glm::mat4(1.0f);

        // Use translation, rotation, and scale to change the initialized matrices
        trans = glm::translate(trans, translation);
        rot = glm::mat4_cast(rotation);
        sca = glm::scale(sca, scale);

        // Multiply all matrices together
        glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

        // Check if the node contains a mesh and if it does load it
        if (node.find("mesh") != node.end())
        {
            model.translationsMeshes.push_back(translation);
            model.rotationsMeshes.push_back(rotation);
            model.scalesMeshes.push_back(scale);
            model.matricesMeshes.push_back(matNextNode);

            LoadMesh(model, node["mesh"]);
        }

        // Check if the node has children, and if it does, apply this function to them with the matNextNode
        if (node.find("children") != node.end())
        {
            for (unsigned int i = 0; i < node["children"].size(); i++)
                TraverseNode(model, node["children"][i], matNextNode);
        }
    }

    void LoadMesh(MODEL::Model& model, unsigned int indMesh)
    {
        // Get all accessor indices
        unsigned int posAccInd = model.JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
        unsigned int normalAccInd = model.JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
        unsigned int texAccInd = model.JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
        unsigned int indAccInd = model.JSON["meshes"][indMesh]["primitives"][0]["indices"];

        // Use accessor indices to get all vertices components
        std::vector<float> posVec = GetFloats(model, model.JSON["accessors"][posAccInd]);
        std::vector<glm::vec3> positions = GroupFloatsVec3(posVec);
        std::vector<float> normalVec = GetFloats(model, model.JSON["accessors"][normalAccInd]);
        std::vector<glm::vec3> normals = GroupFloatsVec3(normalVec);
        std::vector<float> texVec = GetFloats(model, model.JSON["accessors"][texAccInd]);
        std::vector<glm::vec2> texUVs = GroupFloatsVec2(texVec);

        // Combine all the vertex components and also get the indices and textures
        std::vector<Vertex> vertices = AssembleVertices(positions, normals, texUVs);
        std::vector<GLuint> indices = GetIndices(model, model.JSON["accessors"][indAccInd]);
        //std::vector<Texture> textures = GetTextures();

        // Combine the vertices, indices, and textures into a mesh
        //model.meshes.push_back(Mesh(vertices, indices, textures));
    }

    std::vector<float> GetFloats(MODEL::Model& model, RESOURCES::Json accessor)
    {
        std::vector<float> floatVec;

        // Get properties from the accessor
        unsigned int buffViewInd = accessor.value("bufferView", 1);
        unsigned int count = accessor["count"];
        unsigned int accByteOffset = accessor.value("byteOffset", 0);
        std::string type = accessor["type"];

        // Get properties from the bufferView
        RESOURCES::Json bufferView = model.JSON["bufferViews"][buffViewInd];
        unsigned int byteOffset = bufferView["byteOffset"];

        // Interpret the type and store it into numPerVert
        unsigned int numPerVert;
        if (type == "SCALAR") numPerVert = 1;
        else if (type == "VEC2") numPerVert = 2;
        else if (type == "VEC3") numPerVert = 3;
        else if (type == "VEC4") numPerVert = 4;
        else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

        // Go over all the bytes in the data at the correct place using the properties from above
        unsigned int beginningOfData = byteOffset + accByteOffset;
        unsigned int lengthOfData = count * 4 * numPerVert;
        for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i)
        {
            unsigned char bytes[] = { model.data[i++], model.data[i++], model.data[i++], model.data[i++] };
            float value;
            std::memcpy(&value, bytes, sizeof(float));
            floatVec.push_back(value);
        }

        return floatVec;
    }

    std::vector<GLuint> GetIndices(MODEL::Model& model, RESOURCES::Json accessor)
    {
        std::vector<GLuint> indices;

        // Get properties from the accessor
        unsigned int buffViewInd = accessor.value("bufferView", 0);
        unsigned int count = accessor["count"];
        unsigned int accByteOffset = accessor.value("byteOffset", 0);
        unsigned int componentType = accessor["componentType"];

        // Get properties from the bufferView
        RESOURCES::Json bufferView = model.JSON["bufferViews"][buffViewInd];
        unsigned int byteOffset = bufferView["byteOffset"];

        // Get indices with regards to their type: unsigned int, unsigned short, or short
        unsigned int beginningOfData = byteOffset + accByteOffset;
        if (componentType == 5125)
        {
            for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i)
            {
                unsigned char bytes[] = { model.data[i++], model.data[i++], model.data[i++], model.data[i++] };
                unsigned int value;
                std::memcpy(&value, bytes, sizeof(unsigned int));
                indices.push_back((GLuint)value);
            }
        }
        else if (componentType == 5123)
        {
            for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
            {
                unsigned char bytes[] = { model.data[i++], model.data[i++] };
                unsigned short value;
                std::memcpy(&value, bytes, sizeof(unsigned short));
                indices.push_back((GLuint)value);
            }
        }
        else if (componentType == 5122)
        {
            for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
            {
                unsigned char bytes[] = { model.data[i++], model.data[i++] };
                short value;
                std::memcpy(&value, bytes, sizeof(short));
                indices.push_back((GLuint)value);
            }
        }

        return indices;
    }

    std::vector<Vertex> AssembleVertices
            (
                    std::vector<glm::vec3> positions,
                    std::vector<glm::vec3> normals,
                    std::vector<glm::vec2> texUVs
            )
    {
        std::vector<Vertex> vertices;
        for (int i = 0; i < positions.size(); i++)
        {
            vertices.push_back
                    (
                            Vertex
                                    {
                                            positions[i],
                                            normals[i],
                                            glm::vec3(1.0f, 1.0f, 1.0f),
                                            texUVs[i]
                                    }
                    );
        }
        return vertices;
    }

    std::vector<glm::vec2> GroupFloatsVec2(std::vector<float> floatVec)
    {
        std::vector<glm::vec2> vectors;
        for (int i = 0; i < floatVec.size(); i)
        {
            vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
        }
        return vectors;
    }
    std::vector<glm::vec3> GroupFloatsVec3(std::vector<float> floatVec)
    {
        std::vector<glm::vec3> vectors;
        for (int i = 0; i < floatVec.size(); i)
        {
            vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
        }
        return vectors;
    }
    std::vector<glm::vec4> GroupFloatsVec4(std::vector<float> floatVec)
    {
        std::vector<glm::vec4> vectors;
        for (int i = 0; i < floatVec.size(); i)
        {
            vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
        }
        return vectors;
    }

}
