#include "LightSource.h"

using namespace glm;

LightSource::LightSource()
{
	//position.push_back(vec4(-7.07f, 1.0f, 7.07f, 1));

	//color.push_back(vec3(0.0f, 0.8f, 0.8f));

}


LightSource::~LightSource()
{
}



bool LightSource::ParseLine(std::vector<ci_string> token)
{
	if (token.empty())
	{
		return true;
	}
	if (token[0].empty() == false && token[0][0] == '#')
	{
		return true;
	}
	else if (token[0] == "name")
	{
		assert(token.size() > 2);
		assert(token[1] == "=");

		name = token[2];
	}
	else if (token[0] == "position")
	{
		assert(token.size() > 4);
		assert(token[1] == "=");

		position.x = static_cast<float>(atof(token[2].c_str()));
		position.y = static_cast<float>(atof(token[3].c_str()));
		position.z = static_cast<float>(atof(token[4].c_str()));
		position.w = static_cast<float>(atof(token[5].c_str()));

	}
	else if (token[0] == "lightattenuation")
	{
		assert(token.size() > 4);
		assert(token[1] == "=");

		lightAttenuation.x = static_cast<float>(atof(token[2].c_str()));
		lightAttenuation.y = static_cast<float>(atof(token[3].c_str()));
		lightAttenuation.z = static_cast<float>(atof(token[4].c_str()));
	}
	else if (token[0] == "color")
	{
		assert(token.size() > 4);
		assert(token[1] == "=");

		color.x = static_cast<float>(atof(token[2].c_str()));
		color.y = static_cast<float>(atof(token[3].c_str()));
		color.z = static_cast<float>(atof(token[4].c_str()));
	}
	else
	{
		fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
		getchar();
		exit(-1);
	}

	return true;
}

void LightSource::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while (std::getline(iss, line))
	{
		// Splitting line into tokens
		ci_istringstream strstr(line);
		std::istream_iterator<ci_string, char, ci_char_traits> it(strstr);
		std::istream_iterator<ci_string, char, ci_char_traits> end;
		std::vector<ci_string> token(it, end);

		if (ParseLine(token) == false)
		{
			fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
			getchar();
			exit(-1);
		}
	}
}

void LightSource::setPostion(glm::vec4 position)
{
	this->position = position;
}

void LightSource::setColor(glm::vec3 color)
{
	this->color = color;
}
