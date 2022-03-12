#ifndef TEXTURE_H
#define TEXTURE_H

class Texture {
public:
    Texture() = default;

    void create(int width, int height, int slot = 0) {
        glGenTextures(1, &texture);
        this->slot = slot;
        this->width = width;
        this->height = height;
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    void bind() {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void load(void* data) {
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    void bind_as_image() {
        bind();
        glBindImageTexture(slot, texture, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);
    }

    void resize(int width, int height) {
        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

private:
    GLuint texture;
    int slot;
    int width, height;
};

#endif