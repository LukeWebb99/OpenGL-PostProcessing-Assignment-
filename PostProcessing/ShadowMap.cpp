#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
    m_FBO = 0; // int default values
    m_shadowMap = 0;
    m_width = 0;
    m_height = 0;
}

ShadowMap::~ShadowMap()
{
    if (m_FBO) //delete framebuffers
        glDeleteFramebuffers(1, &m_FBO);

    if (m_shadowMap) //delete texture
        glDeleteTextures(1, &m_shadowMap);
}

bool ShadowMap::Init(unsigned int width, unsigned int height) {

    m_height = height; //store size
    m_width = width;

    glGenFramebuffers(1, &m_FBO); //gen framebuffer

    glGenTextures(1, &m_shadowMap); //gen textures
    glBindTexture(GL_TEXTURE_2D, m_shadowMap); //bind texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr); //reserve size of texture and pass no data

    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // set texture properties
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // set boarder colour 
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
   
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); // bind framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0); // attach texture to framebuffer 

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER); // get framebuffer status

    if (status != GL_FRAMEBUFFER_COMPLETE) { // if error
        printf("[SHADOW MAP ERROR]: %i\n", status); // print error
        return false;
    }


    return true;
}

void ShadowMap::BindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); // bind framebuffer
}

void ShadowMap::BindTexture(const int textureUnit)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit); // bind texture to texture unit
    glBindTexture(GL_TEXTURE_2D, m_shadowMap);
}
