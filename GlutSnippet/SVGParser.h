#ifndef SVG_PARSER_H_
#define SVG_PARSER_H_

#include "Common.h"
#include "SVGPathTokenizer.h"

class SVGPath;
class SVGPattern;

class SVGParser
{
public:
	SVGParser();

	~SVGParser();
		

	void parse(const char * fname);
	static void startElement(void * data, const char * el, const char** attr);
	static void endElement(void * data, const char * el);
	static void content(void * data, const char * s, int len);
	void parsePath(const char ** attr);

	SVGPattern * toSVGPattern();
protected:
	void parse_path(path_tokenizer & tok);
	
private:
	std::vector<SVGPath*> paths_;			
	SVGPath * curr_path_;
	char * buf_;
	bool path_flag_;
	path_tokenizer tokenizer_;
};

#endif // SVG_PARSER_H_