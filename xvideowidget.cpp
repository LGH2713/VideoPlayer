#include "XVideoWidget.h"

// 自动加双引号
#define GET_STR(x) #x

#define A_VER 3
#define T_VER 4

// 顶点shader
const char *vString = GET_STR(
            attribute vec4 vertexIn;
        attribute vec2 textureIn;
varying vec2 textureOut;
void main(void)
{
    gl_Position = vertexIn;
    textureOut = textureIn;
}
);

// 片元shader
const char *tString = GET_STR(
            varying vec2 textureOut;
        uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
void main(void)
{
    vec3 yuv;
    vec3 rgb;
    yuv.x = texture2D(tex_y, textureOut).r;
    yuv.y = texture2D(tex_u, textureOut).r - 0.5;
    yuv.z = texture2D(tex_v, textureOut).r - 0.5;
    rgb = mat3(1.0, 1.0, 1.0,
               0.0, -0.39465, 2.03211,
               1.13983, -0.58060, 0.0) * yuv;
    gl_FragColor = vec4(rgb, 1.0);
}
);

XVideoWidget::XVideoWidget(QWidget *parent)
{

}

XVideoWidget::~XVideoWidget()
{

}

void XVideoWidget::initializeGL()
{
    qDebug() << "initializeGL";

    // 初始化openGL函数
    initializeOpenGLFunctions();

    // program加载shader（顶点和片元）脚本
    // 片元（像素）
    qDebug() << program.addShaderFromSourceCode(QOpenGLShader::Fragment, tString);
    // 顶点shader
    qDebug() << program.addShaderFromSourceCode(QOpenGLShader::Vertex, vString);

    // 设置顶点坐标变量
    program.bindAttributeLocation("vertexIn", A_VER);

    // 设置材质坐标
    program.bindAttributeLocation("textureIn", T_VER);

    // 编译shader
    qDebug() << "program.link() = " << program.link();
    qDebug() << "program.bind() = " << program.bind();

    // 传递顶点坐标和材质坐标
    // 顶点
    static const GLfloat ver[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
    };

    // 材质
    static const GLfloat tex[] = {
        0.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
    };

    // 顶点
    glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
    glEnableVertexAttribArray(A_VER);

    // 材质
    glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
    glEnableVertexAttribArray(T_VER);
}

void XVideoWidget::paintGL()
{
    qDebug() << "paintGL";
}

void XVideoWidget::resizeGL(int width, int height)
{
    qDebug() << "paintGL";
}
