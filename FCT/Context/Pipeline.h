#pragma once
#include "./VertexShader.h"
#include "./PixelShader.h"
#include "./VertexFactory.h"
#include "../Shapes/Shape.h"
#include "../Object/Object.h"
namespace FCT {

    class Context;

    class Pipeline {
    public:
        Pipeline(Context* context, VertexFactory* defaultFactory);
        ~Pipeline();
        void begin();
		void end();
        void draw(Shape* shape);
		void draw(Object* object);
        VertexShader* getDefaultVertexShader() const;
        PixelShader* getDefaultPixelShader() const;
		Material* getDefaultMaterial() const;
        VertexFactory* getVertexFactory() const;
        void setDefaultMaterial(Material* material);
        void setPerspective(float fov, float aspect, float nearPlane, float farPlane);
        void setViewMatrix(const Mat4& viewMatrix);
        void lookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
        Context* getContext() const;
    protected:
        Context* m_context;
        VertexShader* m_defaultVertexShader;
        PixelShader* m_defaultPixelShader;
		Material* m_defaultMaterial;
        VertexFactory* m_defaultFactory;
        ConstBuffer* m_viewMatrixConstBuffer;
		ConstBuffer* m_projectionMatrixConstBuffer;
        Mat4 m_viewMatrix;
		Mat4 m_projectionMatrix;
        void createDefaultResource(); 
        void updateProjectionMatrix();
        void updateViewMatrix();
    };

} // namespace FCT
