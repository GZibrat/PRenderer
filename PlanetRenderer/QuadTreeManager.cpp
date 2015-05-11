#include "QuadTreeManager.h"
#include "RingBuffer.h"
#include "SimplexNoise.h"

SimplexNoise1234* snoise;

void finishRequest(struct QuadRequest* req);

RingBuffer(VBOBuffer, GLuint, NUM_VBOS)
};

RingBuffer(TextureBuffer, GLuint, NUM_TEXTURES)
};

struct VBOBuffer vbos;
struct TextureBuffer textures;

RingBuffer(Requests, struct QuadRequest*, MAX_PLANETS)
RB_OPT;
std::mutex readLock;
std::mutex writeLock;
};

struct Requests reqs;

GLuint ibos[16];

std::thread* threads[NUM_THREADS];

#define N_V NUM_FLOATS_PER_VERTEX

void handleRequest()
{
	struct QuadRequest* req;
	std::unique_lock<std::mutex> tailcvLock(reqs.readLock);
	
	while (1)
	{
		RB_GET_SAFE(reqs, req);
		tailcvLock.unlock();
		float* data = req->data;
		QuadTree* tree = req->tree;
		int a = FI[tree->face][0], b = FI[tree->face][1], c = FI[tree->face][2];
		
		//GEN QUAD SPHEREICAL CENTER
		double f = (double)FSI[tree->face % 2];
		tree->sphereCenter[a] = tree->uv.x;
		tree->sphereCenter[b] = tree->uv.y;
		tree->sphereCenter[c] = f;
		double x = tree->sphereCenter.x, y = tree->sphereCenter.y, z = tree->sphereCenter.z;
		tree->sphereCenter.x = tree->scale * x * sqrt(1.0 - (y*y / 2.0) - (z*z / 2.0) + (y*y*z*z / 3.0));
		tree->sphereCenter.y = tree->scale * y * sqrt(1.0 - (z*z / 2.0) - (x*x / 2.0) + (z*z*x*x / 3.0));
		tree->sphereCenter.z = tree->scale * z * sqrt(1.0 - (x*x / 2.0) - (y*y / 2.0) + (x*x*y*y / 3.0));
		tree->toWorld = glm::dmat4(1.0);
		tree->toWorld[3][0] = tree->sphereCenter.x + tree->planet->center.x;
		tree->toWorld[3][1] = tree->sphereCenter.y + tree->planet->center.y;
		tree->toWorld[3][2] = tree->sphereCenter.z + tree->planet->center.z;


		//GEN REST OF POINTS
		glm::dvec3 point;
		glm::dvec3 norm;
		double unit = tree->uw / NODE_SIZE_1;
		double u = tree->uv.x - tree->uw / 2.0;
		double uo = u;
		double v = tree->uv.y - tree->uw / 2.0;

		double s = tree->localUW;

		for (int i = 0; i < NODE_SIZE; i++, v += unit)
		{
			u = uo;
			for (int j = 0; j < NODE_SIZE; j++, u += unit)
			{
				point[a] = u;
				point[b] = v;
				point[c] = f;
				
				double x = point.x, y = point.y, z = point.z;
				point.x = x * sqrt(1.0 - (y*y / 2.0) - (z*z / 2.0) + (y*y*z*z / 3.0));
				point.y = y * sqrt(1.0 - (z*z / 2.0) - (x*x / 2.0) + (z*z*x*x / 3.0));
				point.z = z * sqrt(1.0 - (x*x / 2.0) - (y*y / 2.0) + (x*x*y*y / 3.0));
				data[i * N_V * NODE_SIZE + N_V * j + 8] = (float)snoise->GetHeight(point.x, point.y, point.z);
				//std::cout << data[i * N_V * NODE_SIZE + N_V * j + 8] << " ";
				norm = glm::normalize(point);
				data[i * N_V * NODE_SIZE + N_V * j + 5] = (float)norm.x;
				data[i * N_V * NODE_SIZE + N_V * j + 6] = (float)norm.y;
				data[i * N_V * NODE_SIZE + N_V * j + 7] = (float)norm.z;
				point.x = point.x * tree->scale - tree->sphereCenter.x;
				point.y = point.y * tree->scale - tree->sphereCenter.y;
				point.z = point.z * tree->scale - tree->sphereCenter.z;
				data[i * N_V * NODE_SIZE + N_V * j] = (float)point.x;
				data[i * N_V * NODE_SIZE + N_V * j + 1] = (float)point.y;
				data[i * N_V * NODE_SIZE + N_V * j + 2] = (float)point.z;
				data[i * N_V * NODE_SIZE + N_V * j + 3] = (float)(1.0*j / NODE_SIZE_1*s + tree->localUV.x);
				data[i * N_V * NODE_SIZE + N_V * j + 4] = (float)(1.0*i / NODE_SIZE_1*s + tree->localUV.y);
			}
		}
		tree->sphereCenter.x += tree->planet->center.x;
		tree->sphereCenter.y += tree->planet->center.y;
		tree->sphereCenter.z += tree->planet->center.z;
		//tree->parent->connectNeighbor(tree, tree->side);
		for (int i = 0; i < 4; i++)
		{
			//tree->neighbors[i]->updateState();
		}
		finishRequest(req);
		tailcvLock.lock();
	}
}

void registerRequest(struct QuadRequest* req, float* data, QuadTree* tree)
{
	req->data = data;
	req->done = false;
	req->tree = tree;
	std::unique_lock<std::mutex> headcvLock(reqs.writeLock);
	RB_PUT_SAFE(reqs, req);
}

void waitOnQuadRequest(struct QuadRequest* req)
{
	std::unique_lock<std::mutex> lk(req->mutex);
	while (!req->done)
		req->cv.wait(lk);
}

void finishRequest(struct QuadRequest* req)
{
	req->mutex.lock();
	req->done = true;
	req->mutex.unlock();
	req->cv.notify_all();
}

GLuint getFreeVBO()
{
	GLuint ret;
	RB_GET(vbos, ret, -1);
	return ret;
}

GLuint getFreeTexture()
{
	GLuint ret;
	RB_GET(textures, ret, -1);
	return ret;
}

void putFreeVBO(GLuint vbo)
{
	int ret;
	RB_PUT(vbos, vbo, ret);
}

void putFreeTexture(QuadTree* tree, GLuint texture)
{
	int ret;
	if (!tree->hasTexture) return;
	tree->hasTexture = false;
	RB_PUT(textures, texture, ret);
}

void initQuadManager()
{
	GLuint vbo;
	GLuint texture;
	RB_INIT(vbos, NUM_VBOS);
	RB_INIT(textures, NUM_TEXTURES);
	RB_INIT(reqs, MAX_PLANETS);

	snoise = new SimplexNoise1234(16.0, 5123.123, 1.0, 1, 81);

	for (int i = 0; i < NUM_THREADS; i++)
	{
		threads[i] = new std::thread(handleRequest);
	}

	int ret;
	int i = 0;
	for (; i < NUM_TEXTURES; i++)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, VBO_NUM_BYTES, NULL, GL_DYNAMIC_DRAW);
		RB_PUT(vbos, vbo, ret);

		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		RB_PUT(textures, texture, ret);
	}
	for (; i < NUM_VBOS; i++)
	{
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, VBO_NUM_BYTES, NULL, GL_DYNAMIC_DRAW);
		RB_PUT(vbos, vbo, ret);
	}
}
