#ifndef _CSCI441_MODEL_H_
#define _CSCI441_MODEL_H_

class Model {

public:
    GLuint vbo;
    GLuint vao;
    Shader shader;
    Matrix4 model;
    int size;

    template <typename Coords>
    Model(const Coords& coords, const Shader& shader_in, int type) : shader(shader_in) {
        size = coords.size()*sizeof(float);

        // copy vertex data
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, coords.data(), GL_STATIC_DRAW);

        // describe vertex layout
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
	if(type == 0){

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float),
                (void*)(0*sizeof(float)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float),
                (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float),
                (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);
	}else{
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float),
                (void*)(0*sizeof(float)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float),
                (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11*sizeof(float),
                (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11*sizeof(float),
                (void*)(9*sizeof(float)));
        glEnableVertexAttribArray(3);
	}
    }
};

#endif
