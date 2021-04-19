#include "OmniShadowMap.h"

OmniShadowMap::OmniShadowMap() : ShadowMap() {
}

OmniShadowMap::~OmniShadowMap()
{
}

bool OmniShadowMap::Init(unsigned int width, unsigned int height)
{
    m_width = width; m_height = height; // store width and height

    glGenFramebuffers(1, &m_FBO); // gen framebuffer
    glGenTextures(1, &m_shadowMap); // gen texture for shadow map 

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap); // bind texture

    for (size_t i = 0; i < 6; i++)  // for each face of cubemap texture
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr); // set texture size and pass no data

    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set texture properties 
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); // bind framebuffer object
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowMap, 0); // bind texture to framebuffer

    glDrawBuffer(GL_NONE); // draw buffer none
    glReadBuffer(GL_NONE); // read buffer none

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); // get framebuffer status

    if (status != GL_FRAMEBUFFER_COMPLETE) { // if error 
        printf("[OMNI SHADOW MAP ERROR]: %i\n", status); // print error
        return false; // return false meaning failure
    }
     
    return true; // return true meaning everything is ok
}

void OmniShadowMap::BindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); // bind shadow map framebuffer
}

void OmniShadowMap::BindTexture(const int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit); // bind shadow map texture 
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_shadowMap); 
}
