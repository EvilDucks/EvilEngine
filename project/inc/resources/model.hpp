#pragma once

#include <fstream>
#include "../render/mesh.hpp"
#include "../render/material.hpp"
#include "json.hpp"

namespace MODEL::TEXTURE {
    struct Texture {
        GLuint ID;
        const char* type;
        GLuint unit;
    };

    MODEL::TEXTURE::Texture Create(const char* image, const char* texType, GLuint slot)
    {
        MODEL::TEXTURE::Texture texture{};


        return texture;
    }

}

namespace MODEL::MATERIAL {
    struct Material {
        unsigned int index = 0;
        std::string materialName;
        bool doubleSided = true;
        glm::vec4 baseColorFactor = glm::vec4(1.f);
        float metallicFactor = 0.f;
        float roughnessFactor = 0.f;
    };
}

namespace MODEL::MESH {
    struct Mesh {
        std::vector <Vertex> vertices;
        std::vector <GLuint> indices;
        std::vector <MODEL::TEXTURE::Texture> textures;
        MODEL::MATERIAL::Material material;
        // Store VAO in public so it can be used in the Draw function
        //const GLuint& VAO;
    };

    MODEL::MESH::Mesh Create(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <MODEL::TEXTURE::Texture>& textures, MODEL::MATERIAL::Material& material)
    {
        MODEL::MESH::Mesh mesh{};
        mesh.vertices = vertices;
        mesh.indices = indices;
        mesh.textures = textures;
        mesh.material = material;

        return mesh;
    }
}

namespace MODEL {

    struct Model {
        // Variables for easy access
        const char* file;
        std::vector<unsigned char> data;
        RESOURCES::Json JSON;

        // All the meshes and transformations
        std::vector<MODEL::MESH::Mesh> meshes;
        std::vector<glm::vec3> translationsMeshes;
        std::vector<glm::quat> rotationsMeshes;
        std::vector<glm::vec3> scalesMeshes;
        std::vector<glm::mat4> matricesMeshes;
        std::vector<glm::mat4> nodeMatrices;

        // Prevents textures from being loaded twice
        std::vector<std::string> loadedTexName;
        std::vector<MODEL::TEXTURE::Texture> loadedTex;
        
        std::vector<MODEL::MATERIAL::Material> loadedMaterials;
    };

    std::string Get_file_contents(const char* filename);

    void Create(MODEL::Model& model, const char* file);
    //void Draw(Shader& shader, Camera& camera);

    // Loads a single mesh by its index
    void LoadMesh(MODEL::Model& model, unsigned int indMesh, unsigned int indPrimitives);

    // Traverses a node recursively, so it essentially traverses all connected nodes
    void TraverseNode(MODEL::Model& model);

    // Gets the binary data from a file
    std::vector<unsigned char> GetData(MODEL::Model& model);

    // Interprets the binary data into floats, indices, and textures
    std::vector<float> GetFloats(MODEL::Model& model, RESOURCES::Json accessor);
    std::vector<GLuint> GetIndices(MODEL::Model& model, RESOURCES::Json accessor);
    std::vector<MODEL::TEXTURE::Texture> GetTextures(MODEL::Model& model);

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
    void LoadMaterials(MODEL::Model& model);
    void LoadNodeMatrices(MODEL::Model& model, glm::mat4 matrix = glm::mat4(1.f));

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

        LoadMaterials(model);

        LoadNodeMatrices(model);

        // Traverse all nodes
        MODEL::TraverseNode(model);
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

    void TraverseNode(MODEL::Model& model)
    {
        for (int meshNode = 0; meshNode < model.JSON["meshes"].size(); meshNode++)
        {
            for (int primitiveNode = 0; primitiveNode < model.JSON["meshes"][meshNode]["primitives"].size(); primitiveNode++)
            {
                LoadMesh(model, meshNode, primitiveNode);

            }
        }
    }

    void LoadMesh(MODEL::Model& model, unsigned int indMesh, unsigned int indPrimitives)
    {
        // Get all accessor indices
        unsigned int posAccInd = model.JSON["meshes"][indMesh]["primitives"][indPrimitives]["attributes"]["POSITION"];
        unsigned int normalAccInd = model.JSON["meshes"][indMesh]["primitives"][indPrimitives]["attributes"]["NORMAL"];
        unsigned int texAccInd = model.JSON["meshes"][indMesh]["primitives"][indPrimitives]["attributes"]["TEXCOORD_0"];
        unsigned int indAccInd = model.JSON["meshes"][indMesh]["primitives"][indPrimitives]["indices"];
        unsigned int materialIndex = model.JSON["meshes"][indMesh]["primitives"][indPrimitives]["material"];

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
        std::vector<MODEL::TEXTURE::Texture> textures = GetTextures(model);

        // Combine the vertices, indices, and textures into a mesh
        model.meshes.push_back(MODEL::MESH::Create(vertices, indices, textures, model.loadedMaterials[materialIndex]));
        model.matricesMeshes.push_back(model.nodeMatrices[indMesh]);
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

    std::vector<MODEL::TEXTURE::Texture> GetTextures(MODEL::Model& model)
    {
        std::vector<MODEL::TEXTURE::Texture> textures;

        std::string fileStr = std::string(model.file);
        std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

        // Go over all images
        for (unsigned int i = 0; i < model.JSON["images"].size(); i++)
        {
            // uri of current texture
            std::string texPath = model.JSON["images"][i]["uri"];

            // Check if the texture has already been loaded
            bool skip = false;
            for (unsigned int j = 0; j < model.loadedTexName.size(); j++)
            {
                if (model.loadedTexName[j] == texPath)
                {
                    textures.push_back(model.loadedTex[j]);
                    skip = true;
                    break;
                }
            }

            // If the texture has been loaded, skip this
            if (!skip)
            {
                // Load diffuse texture
                if (texPath.find("baseColor") != std::string::npos)
                {
                    MODEL::TEXTURE::Texture diffuse = MODEL::TEXTURE::Create((fileDirectory + texPath).c_str(), "diffuse", model.loadedTex.size());
                    textures.push_back(diffuse);
                    model.loadedTex.push_back(diffuse);
                    model.loadedTexName.push_back(texPath);
                }
                    // Load specular texture
                else if (texPath.find("metallicRoughness") != std::string::npos)
                {
                    MODEL::TEXTURE::Texture specular = MODEL::TEXTURE::Create((fileDirectory + texPath).c_str(), "specular", model.loadedTex.size());
                    textures.push_back(specular);
                    model.loadedTex.push_back(specular);
                    model.loadedTexName.push_back(texPath);
                }
            }
        }

        return textures;
    }
    
    void LoadMaterials(MODEL::Model& model)
    {
        for (int i = 0; i < model.JSON["materials"].size(); i++)
        {
            bool doubleSided = model.JSON["materials"][i]["doubleSided"];
            std::string materialName = model.JSON["materials"][i]["name"];
            float baseColorR = model.JSON["materials"][i]["pbrMetallicRoughness"]["baseColorFactor"][0];
            float baseColorG = model.JSON["materials"][i]["pbrMetallicRoughness"]["baseColorFactor"][1];
            float baseColorB = model.JSON["materials"][i]["pbrMetallicRoughness"]["baseColorFactor"][2];
            float baseColorA = model.JSON["materials"][i]["pbrMetallicRoughness"]["baseColorFactor"][3];
            float metallicFactor = model.JSON["materials"][i]["pbrMetallicRoughness"]["metallicFactor"];
            float roughnessFactor = model.JSON["materials"][i]["pbrMetallicRoughness"]["roughnessFactor"];

            model.loadedMaterials.emplace_back(MODEL::MATERIAL::Material(i, materialName, doubleSided, glm::vec4(baseColorR, baseColorG, baseColorB, baseColorA), metallicFactor, roughnessFactor));
        }
    }

    void LoadNodeMatrices(MODEL::Model& model, glm::mat4 matrix)
    {
        for (int nextNode = 0; nextNode < model.JSON["nodes"].size(); nextNode++)
        {
            RESOURCES::Json node = model.JSON["nodes"][nextNode];
            std::string nodeName = node["name"];
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
            model.nodeMatrices.push_back(matNextNode);
        }
    }
}
