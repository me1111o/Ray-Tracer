#ifndef _TEXTSCENEGRAPHRENDERER_H_
#define _TEXTSCENEGRAPHRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include "RotateTransform.h"
#include <sstream>
#include <vector>
#include <string>

namespace sgraph
{

    class TextScenegraphRenderer : public SGNodeVisitor
    {
    public:
        TextScenegraphRenderer() : level(0) {}

        std::string getOutput() const
        {
            return output.str();
        }

        virtual void visitGroupNode(GroupNode *node) override
        {
            printIndent();
            output << node->getName() << std::endl;
            level++;
            std::vector<SGNode *> children = node->getChildren();
            for (size_t i = 0; i < children.size(); i++)
            {
                children[i]->accept(this);
            }
            level--;
        }

        virtual void visitLeafNode(LeafNode *node) override
        {
            printIndent();
            output << node->getName() << std::endl;
        }

        virtual void visitTransformNode(TransformNode *node) override
        {
            printIndent();
            output << node->getName() << " (transform)" << std::endl;
            level++;
            if (!node->getChildren().empty())
            {
                node->getChildren()[0]->accept(this);
            }
            level--;
        }

        virtual void visitScaleTransform(ScaleTransform *node) override
        {
            printIndent();
            auto scaleVec = node->getScale();
            output << node->getName() << " (scale "
                   << scaleVec.x << ", " << scaleVec.y << ", " << scaleVec.z << ")" << std::endl;
            level++;
            if (!node->getChildren().empty())
            {
                node->getChildren()[0]->accept(this);
            }
            level--;
        }

        virtual void visitTranslateTransform(TranslateTransform *node) override
        {
            printIndent();
            auto transVec = node->getTranslate();
            output << node->getName() << " (translate "
                   << transVec.x << ", " << transVec.y << ", " << transVec.z << ")" << std::endl;
            level++;
            if (!node->getChildren().empty())
            {
                node->getChildren()[0]->accept(this);
            }
            level--;
        }

        virtual void visitRotateTransform(RotateTransform *node) override
        {
            printIndent();
            output << node->getName() << " (rotate "
                   << node->getAngleInRadians() << " rad, axis: "
                   << node->getRotationAxis().x << ", "
                   << node->getRotationAxis().y << ", "
                   << node->getRotationAxis().z << ")" << std::endl;
            level++;
            if (!node->getChildren().empty())
            {
                node->getChildren()[0]->accept(this);
            }
            level--;
        }

    private:
        int level;
        std::ostringstream output;

        void printIndent()
        {
            for (int i = 0; i < level; i++)
            {
                output << "\t";
            }
            output << "- ";
        }
    };
}

#endif
