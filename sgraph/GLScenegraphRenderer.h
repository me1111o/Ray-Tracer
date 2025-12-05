#ifndef _GLSCENEGRAPHRENDERER_H_
#define _GLSCENEGRAPHRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include <stack>
#include <iostream>
using namespace std;

namespace sgraph
{
    /**
     * This visitor implements drawing the scene graph using OpenGL
     *
     */
    class GLScenegraphRenderer : public SGNodeVisitor
    {
    public:
        /**
         * @brief Construct a new GLScenegraphRenderer object
         *
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         */
        GLScenegraphRenderer(stack<glm::mat4> &mv,
                             map<string, util::ObjectInstance *> &os,
                             util::ShaderLocationsVault &shaderLocations,
                             const std::map<std::string, GLuint> &textures)
            : modelview(mv), objects(os), textures(textures)
        {
            this->shaderLocations = shaderLocations;
            for (auto it = objects.begin(); it != objects.end(); it++)
            {
                std::cout << "Mesh with name: " << it->first << std::endl;
            }
        }

        /**
         * @brief Recur to the children for drawing
         *
         * @param groupNode
         */
        void visitGroupNode(GroupNode *groupNode)
        {
            for (int i = 0; i < groupNode->getChildren().size(); i = i + 1)
            {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief Draw the instance for the leaf, after passing the
         * modelview and color to the shader
         *
         * @param leafNode
         */
        void visitLeafNode(LeafNode *leafNode)
        {
            // send modelview matrix to GPU
            glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview.top()));

            util::Material mat = leafNode->getMaterial();

            glUniform3fv(shaderLocations.getLocation("materialAmbient"), 1, glm::value_ptr(mat.getAmbient()));
            glUniform3fv(shaderLocations.getLocation("materialDiffuse"), 1, glm::value_ptr(mat.getDiffuse()));
            glUniform3fv(shaderLocations.getLocation("materialSpecular"), 1, glm::value_ptr(mat.getSpecular()));
            glUniform1f(shaderLocations.getLocation("materialShininess"), mat.getShininess());

            // glUniform4fv(shaderLocations.getLocation("vColor"),1,glm::value_ptr(leafNode->getMaterial().getAmbient()));

            std::string texName = leafNode->getTexture();
            if (texName.empty())
            {
                texName = "white";
            }
            auto it = textures.find(texName);
            if (it != textures.end())
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, it->second);
            }
            else
            {
                auto defIt = textures.find("white");
                if (defIt != textures.end())
                {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, defIt->second);
                }
            }
            glUniform1i(shaderLocations.getLocation("textureSampler"), 0);

            objects[leafNode->getInstanceOf()]->draw();
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         *
         * @param transformNode
         */
        void visitTransformNode(TransformNode *transformNode)
        {
            modelview.push(modelview.top());
            modelview.top() = modelview.top() * transformNode->getTransform();
            if (transformNode->getChildren().size() > 0)
            {
                transformNode->getChildren()[0]->accept(this);
            }
            modelview.pop();
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         *
         * @param scaleNode
         */
        void visitScaleTransform(ScaleTransform *scaleNode)
        {
            visitTransformNode(scaleNode);
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         *
         * @param translateNode
         */
        void visitTranslateTransform(TranslateTransform *translateNode)
        {
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode)
        {
            visitTransformNode(rotateNode);
        }

    private:
        stack<glm::mat4> &modelview;
        util::ShaderLocationsVault shaderLocations;
        map<string, util::ObjectInstance *> objects;

        std::map<std::string, GLuint> textures;
    };
}

#endif