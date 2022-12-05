#include <iostream>
#include <fstream>
#include <string>

void convXml2Svg(std::ifstream &, std::ofstream &);

bool fileExists(const char *fileName)
{
	std::ifstream infile { fileName };
	return infile.good();
}

// usage: svgconv input-file output-file
int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " input-file output-file\n";
		return 1;
	}

	std::ifstream inf { argv[1] };
	if (fileExists(argv[2])) {
		char c;
		std::cout << "About to overwrite file " << argv[2] << ". Proceed? (Y/N)\n";
		std::cin >> c;
		if (!(c == 'y' || c == 'Y')) {
			std::cerr << "Exited.\n";
			return 1;
		}
	}
	std::ofstream outf { argv[2] };

	if (!(inf.is_open() && outf.is_open())) {
		std::cerr << "Couldn't open files.\n";
		return 1;
	}

	convXml2Svg(inf, outf);
	std::cout << "Created file " << argv[2] << " successfully.\n";

	inf.close();
	outf.close();
	return 0;
}
/* 
   convXml2Svg: Copies the content from xml into svg, changing the text as 
   needed (for example, android:pathData is replaced by d) 
*/
void convXml2Svg(std::ifstream &xml, std::ofstream &svg)
{
	void replField(std::string &, const std::string &, const std::string &);

	std::string line;
	double viewportWidth = -1, viewportHeight = -1;
	bool viewBoxWritten = false;

	const char VIEWPORT_WIDTH[] = "android:viewportWidth";
	const char VIEWPORT_HEIGHT[] = "android:viewportHeight";

	char *rem = nullptr;

	while (std::getline(xml, line)) {
		if (line.find("<?xml") != std::string::npos)
			continue;

		replField(line, "vector", "svg");
		replField(line, "xmlns:android=\"http://schemas.android.com/apk/res/android\"", "xmlns=\"http://www.w3.org/2000/svg\"");
		replField(line, "android:pathData", "d");
		replField(line, "android:fillColor", "fill");
		replField(line, "android:strokeColor", "stroke");
		replField(line, "android:strokeWidth", "stroke-width");
		replField(line, "android:width", "width");
		replField(line, "android:height", "height");

		size_t pos;

		if (viewportWidth == -1 && (pos = line.find(VIEWPORT_WIDTH)) != std::string::npos) {
			viewportWidth = strtod(line.c_str() + pos + (sizeof VIEWPORT_WIDTH) + 2, &rem);
			if (rem[0] == '\"')
				++rem;
			continue;
		}

		if (viewportHeight == -1 && (pos = line.find(VIEWPORT_HEIGHT)) != std::string::npos) {
			viewportHeight = strtod(line.c_str() + pos + (sizeof VIEWPORT_HEIGHT) + 2, &rem);
			if (rem[0] == '\"')
				++rem;
			continue;
		}

		if (!viewBoxWritten && viewportHeight != -1 && viewportWidth != -1) {
			svg << "viewBox=\"0 0 " << viewportWidth << " " << viewportHeight << "\"" << (rem ? rem : "") << "\n";
			viewBoxWritten = true;
		}

		svg << line << "\n";
	}
}

/*
  replField: Replaces all instances of the substring old in line line with text in repl
*/
void replField(std::string &line, const std::string &old, const std::string &repl)
{
	size_t pos;

	while ((pos = line.find(old)) != std::string::npos)
		line.replace(pos, old.length(), repl);
}

