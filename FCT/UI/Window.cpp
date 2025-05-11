#include "../FCTAPI.h"

void FCT::Window::registerHandler(EventHandler* handler)
{
	m_handlers.push_back(handler);
	handler->addRef();
}

void FCT::Window::unregisterHandler(EventHandler* handler)
{
	auto it = std::find(m_handlers.begin(), m_handlers.end(), handler);
	if (it != m_handlers.end()) {
		m_handlers.erase(it);
	}
	handler->release();
}

void FCT::Window::clearHandler()
{
	for (auto handler : m_handlers) {
		handler->release();
	}
	m_handlers.clear();
}

void FCT::Window::initRender()
{
	m_swapchain->acquireFirstImage();
}



