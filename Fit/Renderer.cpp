#include "Renderer.h"


LIGHTING_TYPE Renderer::lightType = LIGHTING_TYPE_SH_UNSHADOWED;
LFLOAT Renderer::theta = 54.7f;
LFLOAT Renderer::phi = 19.6f;
LFLOAT Renderer::preMouseX = 0.0f;
LFLOAT Renderer::preMouseY = 0.0f;
bool Renderer::isNeedRotate = false;
bool Renderer::isLeftButtonPress = false;

Renderer::Renderer(int _width, int _height) : width(_width), height(_height)
{
	initGLFW();
	initGL();
}

void Renderer::initGL()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		cerr << "Failed to init glew : " << glewGetErrorString(err) << endl;
		exit(-1);
	}

	glViewport(0, 0, width, height);
	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);
	/* Set the background */
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	/* Depth buffer setup */
	glClearDepth(1.0f);
	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);
	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);
	///* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	modelMatrix = mat4(1.0f);
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), vec3(1.0, 0.0, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(-50.0f), vec3(0.0, 1.0, 0.0));

	modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), vec3(0.0, 1.0, 0.0));

	viewMatrix = glm::lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 1.0f, 0.0f));
}

void Renderer::initGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, "LDPRT", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
}

void Renderer::renderSamples(Sampler &sampler)
{
	glPointSize(POINT_SIZE);
	while (!glfwWindowShouldClose(window))
	{
		if (Renderer::lightType == LIGHTING_TYPE_SH_SHADOWED)break;
		glfwPollEvents();
		GLuint vao, vbo, ebo;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SAMPLE)* sampler.samples.size(), &sampler.samples[0], GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SAMPLE), (GLvoid *)0);

		glEnableVertexAttribArray(0);


#ifdef IADOUBLE
		glVertexAttribPointer(1, 1, GL_DOUBLE, GL_FALSE, sizeof(SAMPLE), (GLvoid *)offsetof(SAMPLE, colorF));
#else
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(SAMPLE), (GLvoid *)offsetof(SAMPLE, approximateColorF));
#endif

		glEnableVertexAttribArray(1);
		glDrawArrays(GL_POINTS, 0, sampler.samples.size());
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
}



void Renderer::compileShaderFromFile(const char *verFileName, const char *fragFileName)
{
	bool ret = prog.compileShaderFromFile(verFileName, GLSLShader::VERTEX);
	if (!ret)
	{
		cerr << "Compile Vertex Shader Error: " << prog.log() << endl;
		exit(-1);
	}
	ret = prog.compileShaderFromFile(fragFileName, GLSLShader::FRAGMENT);
	if (!ret)
	{
		cerr << "Compile Fragment Shader Error: " << prog.log() << endl;
		exit(-1);
	}

	prog.bindAttribLocation(0, "position");
	prog.bindAttribLocation(1, "color");

	ret = prog.link();
	if (!ret)
	{
		cerr << "Link Error: " << prog.log() << endl;
		exit(-1);
	}

	ret = prog.validate();
	if (!ret)
	{
		cerr << "Validate Error: " << prog.log() << endl;
		exit(-1);
	}

	prog.use();
}

void Renderer::setUniform()
{
	glfwGetFramebufferSize(window, &width, &height);
	mat4 projection = glm::perspective(45.0f, float(width) / height, 0.1f, 1000.0f);
	mat4 mv = viewMatrix * modelMatrix;

	prog.setUniform("MVP", projection * mv);
}


void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
		/*case GLFW_KEY_SPACE:
		if (action == GLFW_PRESS) {
		cout << "set animation\n" << endl;
		bAnim = !bAnim;
		}
		break;*/
	case GLFW_KEY_1:
		lightType = LIGHTING_TYPE_SH_UNSHADOWED;
		break;
	case GLFW_KEY_2:
		lightType = LIGHTING_TYPE_SH_SHADOWED;
		break;
	case GLFW_KEY_3:
		lightType = LIGHTING_TYPE_SH_SHADOWED_BOUNCE_1;
		break;
	case GLFW_KEY_4:
		lightType = LIGHTING_TYPE_SH_SHADOWED_BOUNCE_2;
		break;
	case GLFW_KEY_5:
		lightType = LIGHTING_TYPE_SH_SHADOWED_BOUNCE_3;
		break;
	}
}

void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT)return;
	if (action == GLFW_PRESS) {
		isLeftButtonPress = true;
	}
	else {
		isLeftButtonPress = false;
	}
}

void Renderer::cursorPositionCallback(GLFWwindow* window, double x, double y)
{
	theta += (((LFLOAT)y - preMouseY) * 0.1f);
	if (theta < 0.0f)theta = 0.0f;
	if (theta > 180.0f)theta = 180.0f;
	phi += (((LFLOAT)x - preMouseX) * 0.1f);

	preMouseX = (LFLOAT)x;
	preMouseY = (LFLOAT)y;

	isNeedRotate = true;
}
Renderer::~Renderer()
{

}