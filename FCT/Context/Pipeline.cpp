#include "../headers.h"
#include <iostream>

namespace FCT {

    Pipeline::Pipeline(Context* context, VertexFactory* defaultFactory)
        : m_context(context), m_defaultVertexShader(nullptr), m_defaultPixelShader(nullptr), m_defaultFactory(defaultFactory) {
        createDefaultResource();
    }

    Pipeline::~Pipeline() {
        delete m_defaultVertexShader;
        delete m_defaultPixelShader;
    }

    void Pipeline::begin()
    {
        m_defaultMaterial->bind();
        m_viewMatrixConstBuffer->bind();
        m_projectionMatrixConstBuffer->bind();
	}

    void Pipeline::end()
    {
    }

    void Pipeline::draw(Shape* shape)
    {
		shape->draw();
	}

    void Pipeline::draw(Object* object)
    {
		object->draw();
    }

    VertexShader* Pipeline::getDefaultVertexShader() const {
        return m_defaultVertexShader;
    }

    PixelShader* Pipeline::getDefaultPixelShader() const {
        return m_defaultPixelShader;
    }

    Material* Pipeline::getDefaultMaterial() const
    {
		return m_defaultMaterial;
    }
    VertexFactory* Pipeline::getVertexFactory() const
    {
		return m_defaultFactory;
    }
    void Pipeline::setDefaultMaterial(Material* material)
    {
		m_defaultMaterial = material;
    }
    void Pipeline::setPerspective(float fov, float aspect, float nearPlane, float farPlane) {
        float tanHalfFov = tan(fov / 2.0f);

        m_projectionMatrix = Mat4(
            1.0f / (aspect * tanHalfFov), 0, 0, 0,
            0, 1.0f / tanHalfFov, 0, 0,
            0, 0, -(farPlane + nearPlane) / (farPlane - nearPlane), -1,
            0, 0, -(2 * farPlane * nearPlane) / (farPlane - nearPlane), 0
        );

        updateProjectionMatrix();
    }

    void Pipeline::setViewMatrix(const Mat4& viewMatrix) {
        m_viewMatrix = viewMatrix;
        updateViewMatrix();
    }

    void Pipeline::lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 zaxis = normalize(eye - target);
        Vec3 xaxis = normalize(cross(up, zaxis));
        Vec3 yaxis = cross(zaxis, xaxis);

        m_viewMatrix = Mat4(
            xaxis.x, yaxis.x, zaxis.x, 0,
            xaxis.y, yaxis.y, zaxis.y, 0,
            xaxis.z, yaxis.z, zaxis.z, 0,
            -dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1
        );

        updateViewMatrix();
    }

    Context* Pipeline::getContext() const
    {
		return m_context;
    }

    void Pipeline::updateProjectionMatrix() {
        if (m_projectionMatrixConstBuffer) {
            m_projectionMatrixConstBuffer->setData(&m_projectionMatrix, sizeof(Mat4));
        }
    }

    void Pipeline::updateViewMatrix() {
        if (m_viewMatrixConstBuffer) {
            m_viewMatrixConstBuffer->setData(&m_viewMatrix, sizeof(Mat4));
        }
    }

    void Pipeline::createDefaultResource() {
        m_defaultVertexShader = m_context->createVertexShader(m_defaultFactory);
        if (!m_defaultVertexShader->compileFromSource("")) {
            std::cout << m_defaultVertexShader->getCompileError() << std::endl;
        }

        m_defaultPixelShader = m_context->createPixelShader(m_defaultVertexShader->getOutput());
		if (!m_defaultPixelShader->compileFromSource("")) {
			std::cout << m_defaultPixelShader->getCompileError() << std::endl;
		}

		m_defaultMaterial = m_context->createMaterial(m_defaultVertexShader, m_defaultPixelShader);
        m_defaultMaterial->compile();

        m_viewMatrixConstBuffer = m_context->createConstBuffer();
        m_viewMatrixConstBuffer->create(sizeof(Mat4), 1);
        m_viewMatrixConstBuffer->setData(&m_viewMatrix, sizeof(Mat4));
        m_projectionMatrixConstBuffer = m_context->createConstBuffer();
		m_projectionMatrixConstBuffer->create(sizeof(Mat4), 2);
		m_projectionMatrixConstBuffer->setData(&m_projectionMatrix, sizeof(Mat4));
    }

} // namespace FCT
