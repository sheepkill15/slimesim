#ifndef SSBO_H
#define SSBO_H

class Ssbo
{
public:
    Ssbo(int size, void* data)
    {
        glGenBuffers(1, &ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void bind(int slot = 0) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, ssbo);
    }

    void resize(int size, int slot = 0) {
        bind(slot);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_STATIC_DRAW);
    }

private:
    GLuint ssbo;
};

#endif