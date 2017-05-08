/*
 * Renderable.h
 *
 *  Created on: 2013.01.23
 *      Author: Martynas Šustavičius
 *
 */

#ifndef VERTEX_H_
#define VERTEX_H_

#include "../Multiplatform/Ghost.h"

struct VertexPNT {
	float pos[3];
	float normals[3];
	float uv[2];

	VertexPNT() {
		fill(pos, pos + 3, 0.0f);
		fill(normals, normals + 3, 0.0f);
		fill(uv, uv + 2, 0.0f);
	}
	VertexPNT(const VertexPNT& vert) {
		memcpy(pos, vert.pos, sizeof(pos));
		memcpy(normals, vert.normals, sizeof(normals));
		memcpy(uv, vert.uv, sizeof(uv));
	}
	bool operator ==(const VertexPNT& vert) const
	{
		if (memcmp(pos, vert.pos, sizeof(pos)) != 0) {
			return false;
		}
		if (memcmp(normals, vert.normals, sizeof(normals)) != 0) {
			return false;
		}
		if (memcmp(uv, vert.uv, sizeof(uv)) != 0) {
			return false;
		}
		return true;
	}
	string toString() {
		static ostringstream oss;
		oss.str(string());
		oss << pos[0] << pos[1] << pos[2]
			<< normals[0] << normals[1] << normals[2]
			<< uv[0] << uv[1];
		return oss.str();
	}
};

struct VertexP {
	float pos[3];

	VertexP() {
		fill(pos, pos + 3, 0.0f);
	}
	VertexP(const VertexP& vert) {
		memcpy(pos, vert.pos, sizeof(pos));
	}
	VertexP(const VertexPNT& vert) {
		memcpy(pos, vert.pos, sizeof(pos));
	}
    VertexP(float x, float y, float z) {
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }
	bool operator ==(const VertexP& vert) {
		if (memcmp(pos, vert.pos, sizeof(pos)) != 0) {
			return false;
		}
		return true;
	}
	string toString() {
		static ostringstream oss;
		oss.str(string());
		oss << pos[0] << pos[1] << pos[2];
		return oss.str();
	}
};

struct VertexT {
	float uv[2];

	VertexT() {
		fill(uv, uv + 2, 0.0f);
	}
	VertexT(const VertexT& vert) {
		memcpy(uv, vert.uv, sizeof(uv));
	}
	VertexT(const VertexPNT& vert) {
		memcpy(uv, vert.uv, sizeof(uv));
	}
	bool operator ==(const VertexT& vert) {
		if (memcmp(uv, vert.uv, sizeof(uv)) != 0) {
			return false;
		}
		return true;
	}
	string toString() {
		static ostringstream oss;
		oss.str(string());
		oss << uv[0] << uv[1];
		return oss.str();
	}
};

struct VertexPN {
	float pos[3];
	float normals[3];

	VertexPN() {
		fill(pos, pos + 3, 0.0f);
		fill(normals, normals + 3, 0.0f);
	}
	VertexPN(const VertexPN& vert) {
		memcpy(pos, vert.pos, sizeof(pos));
		memcpy(normals, vert.normals, sizeof(normals));
	}
	VertexPN(const VertexPNT& vert) {
		memcpy(pos, vert.pos, sizeof(pos));
		memcpy(normals, vert.normals, sizeof(normals));
	}
	bool operator ==(const VertexPN& vert) {
		if (memcmp(pos, vert.pos, sizeof(pos)) != 0) {
			return false;
		}
		if (memcmp(normals, vert.normals, sizeof(normals)) != 0) {
			return false;
		}
		return true;
	}
	string toString() {
		static ostringstream oss;
		oss.str(string());
		oss << pos[0] << pos[1] << pos[2]
			<< normals[0] << normals[1] << normals[2];
		return oss.str();
	}
};

struct VertexPT {
	float pos[3];
	float uv[2];

	VertexPT() {
		fill(pos, pos + 3, 0.0f);
		fill(uv, uv + 2, 0.0f);
	}
	VertexPT(const VertexPT& vert) {
		memcpy(pos, vert.pos, sizeof(pos));
		memcpy(uv, vert.uv, sizeof(uv));
	}
	VertexPT(const VertexPNT& vert) {
		memcpy(pos, vert.pos, sizeof(pos));
		memcpy(uv, vert.uv, sizeof(uv));
	}
	bool operator ==(const VertexPT& vert) {
		if (memcmp(pos, vert.pos, sizeof(pos)) != 0) {
			return false;
		}
		if (memcmp(uv, vert.uv, sizeof(uv)) != 0) {
			return false;
		}
		return true;
	}
	string toString() {
		static ostringstream oss;
		oss.str(string());
		oss << pos[0] << pos[1] << pos[2]
			<< uv[0] << uv[1];
		return oss.str();
	}
};

#endif
