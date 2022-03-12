#ifndef UBO_H
#define UBO_H

class Ubo {
public:
    Ubo(int size, void* data) {
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void bind(int slot = 0) { 
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBindBufferBase(GL_UNIFORM_BUFFER, slot, ubo);
    }

    GLuint get() { return ubo; }

private:
    GLuint ubo;
};

#endif