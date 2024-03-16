#pragma once
#include "tool/debug.hpp"
#include "global.hpp"
#include "gl.hpp"

namespace SHADER {

    void GetShaderError (const GLuint& identifier, const char* const type);
    void ReadShader (char*& buffor, const char*& filepath);
    void Create (GLuint& identifier, const char*& filepathVertex, const char*& filepathFragment);
    void Use ();


    void GetShaderError (
        const GLuint& identifier, 
        const char* const type
    ) {
        char infoLog[512];
		GLint isSuccess;
    
		glGetShaderiv (identifier, GL_COMPILE_STATUS, &isSuccess);

		if (!isSuccess) {
			glGetShaderInfoLog (identifier, 512, NULL, infoLog);
			spdlog::error ("{0} Shader Compilation Failure!\n{1}", type, infoLog);
		} else {
			spdlog::info ("Succesfull {0} Shader Compilation", type);
		}
    }


    void ReadShader (
        char*& buffor, 
        const char*& filepath
    ) {
        FILE *file = fopen (filepath, "rb");

        DEBUG {
            u64 length;

            if (file) {
                fseek (file, 0, SEEK_END);
                length = ftell (file);

                fseek (file, 0, SEEK_SET);
                buffor = (char*) malloc (length + 1);
            } else {
                spdlog::error ("Shader: Script not found!");
                exit (1);
            }

            if (buffor) {
                fread (buffor, 1, length, file);
                buffor[length] = '\0';
            } else {
                spdlog::error ("Shader: Buffor could not be allocated!");
                exit (1);
            }
        } else {
            fseek (file, 0, SEEK_END);
            u64 length = ftell (file);

            fseek (file, 0, SEEK_SET);
            buffor = (char*) malloc (length + 1);

            fread (buffor, 1, length, file);
            buffor[length] = '\0';
        }

        fclose (file);
    }


    void Create (
        GLuint& identifier, 
        const char*& filepathVertex, 
        const char*& filepathFragment
    ) {
        GLuint idFragment = glCreateShader (GL_FRAGMENT_SHADER);
        GLuint idVertex = glCreateShader (GL_VERTEX_SHADER);

        char* buffor = nullptr;

        { // VERTEX
            ReadShader (buffor, filepathVertex);
            
		    glShaderSource (idVertex, 1, &buffor, NULL);
		    glCompileShader (idVertex);
            free (buffor);

		    DEBUG { GetShaderError (idVertex, "Vertex"); }
        }

        { // FRAGMENT
            ReadShader (buffor, filepathFragment);

            glShaderSource (idFragment, 1, &buffor, NULL);
            glCompileShader (idFragment);
            free (buffor);

            DEBUG { GetShaderError (idFragment, "Fragment"); }
        }

        { // BUNDLED
            identifier = glCreateProgram ();
            glAttachShader (identifier, idVertex);
            glAttachShader (identifier, idFragment);
            glLinkProgram (identifier);

            DEBUG {
                char infoLog[512];
		        GLint isSuccess;

                glGetProgramiv (identifier, GL_LINK_STATUS, &isSuccess);
                
                if (!isSuccess) {
                    glGetProgramInfoLog (identifier, 512, NULL, infoLog);
                    spdlog::error ("Program Shader Compilation Failure!\n{0}", infoLog);
		        } else {
			        spdlog::info ("Succesfull Program Shader Compilation");
		        }
            }

            // Once objects are linked they're no longer needed.
            glDeleteShader (idVertex);
            glDeleteShader (idFragment);  

        }
        
    }


    void Use (const GLuint& identifier) {
        glUseProgram (identifier);
    }

}