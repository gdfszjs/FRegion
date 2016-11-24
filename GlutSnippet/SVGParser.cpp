#include "SVGParser.h"
#include "expat.h"
#include "SVGPath.h"
#include "CommonGl.h"
#include "SVGPattern.h"

SVGParser::SVGParser()
{
	buf_ = new char[BUFSIZ];
	path_flag_ = false;
}

SVGParser::~SVGParser()
{
	for(size_t i = 0;i<paths_.size();i++)
		delete paths_[i];
	paths_.clear();
	delete buf_;
}

void SVGParser::startElement(void * data, const char * el, const char** attr)
{
	SVGParser& self = *(SVGParser*)data;
	
	/*if(strcmp(el, "g") == 0)
	{
		self.m_path.push_attr();
		self.parse_attr(attr);
	}
	else*/
	if(strcmp(el, "path") == 0)
	{
		if(self.path_flag_)
		{
			cout<<"Nested path..."<<endl;
			return;
		}
		self.path_flag_ = true;

		self.curr_path_ = new SVGPath();

		//self.m_path.begin_path();
		self.parsePath(attr);
		//self.m_path.end_path();
	}
	//else
	//if(strcmp(el, "<OTHER_ELEMENTS>") == 0) 
	//{
	//}
	// . . .
}

void SVGParser::endElement(void * data, const char * el)
{
	SVGParser& self = *(SVGParser*)data;

	/*if(strcmp(el, "title") == 0)
	{
		self.m_title_flag = false;
	}
	else
	if(strcmp(el, "g") == 0)
	{
		self.m_path.pop_attr();
	}
	else*/
	if(strcmp(el, "path") == 0)
	{
		self.path_flag_ = false;
		self.paths_.push_back(self.curr_path_);
	}
	//else
	//if(strcmp(el, "<OTHER_ELEMENTS>") == 0) 
	//{
	//}
	// . . .
}

void SVGParser::content(void * data, const char * s, int len)
{
}

void SVGParser::parsePath(const char ** attr)
{
	int i;

	for(i = 0; attr[i]; i += 2)
	{
		// The <path> tag can consist of the path itself ("d=") 
		// as well as of other parameters like "style=", "transform=", etc.
		// In the last case we simply rely on the function of parsing 
		// attributes (see 'else' branch).
		if(strcmp(attr[i], "d") == 0)
		{
			tokenizer_.set_path_str(attr[i + 1]);
			parse_path(tokenizer_);
		}
		else
		{
			// Create a temporary single pair "name-value" in order
			// to avoid multiple calls for the same attribute.
			/*const char* tmp[4];
			tmp[0] = attr[i];
			tmp[1] = attr[i + 1];
			tmp[2] = 0;
			tmp[3] = 0;
			parse_attr(tmp);*/
		}
	}
}

void SVGParser::parse_path(path_tokenizer & tok)
{
	while(tok.next())
	{
		double arg[10];
		char cmd = tok.last_command();
		unsigned i;
		switch(cmd)
		{
			case 'M': case 'm':
			{	
				arg[0] = tok.last_number();
				arg[1] = tok.next(cmd);
				v2d tmp;
				tmp[0] = arg[0];
				tmp[1] = arg[1];
				SVGPathCommandM * c = new SVGPathCommandM(tmp);
				curr_path_->addCommand(c);
				break;
			}

			case 'L': case 'l':
			{
				arg[0] = tok.last_number();
				arg[1] = tok.next(cmd);
				v2d tmp;
				tmp[0] = arg[0];
				tmp[1] = arg[1];
				SVGPathCommandL * c = new SVGPathCommandL(tmp);
				curr_path_->addCommand(c);
				break; 
			}

			/*case 'V': case 'v':
				vline_to(tok.last_number(), cmd == 'v');
				break;

			case 'H': case 'h':
				hline_to(tok.last_number(), cmd == 'h');
				break;
			
			case 'Q': case 'q':
				arg[0] = tok.last_number();
				for(i = 1; i < 4; i++)
				{
					arg[i] = tok.next(cmd);
				}
				curve3(arg[0], arg[1], arg[2], arg[3], cmd == 'q');
				break;

			case 'T': case 't':
				arg[0] = tok.last_number();
				arg[1] = tok.next(cmd);
				curve3(arg[0], arg[1], cmd == 't');
				break; */

			case 'C': case 'c':
				{
					arg[0] = tok.last_number();
					for(i = 1; i < 6; i++)
					{
						arg[i] = tok.next(cmd);
					}
					v6d tmp;
					tmp[0] = arg[0];
					tmp[1] = arg[1];
					tmp[2] = arg[2];
					tmp[3] = arg[3];
					tmp[4] = arg[4];			
					tmp[5] = arg[5];
					SVGPathCommandC * c = new SVGPathCommandC(tmp);
					curr_path_->addCommand(c);	
					break;
				}

			/*case 'S': case 's':
				arg[0] = tok.last_number();
				for(i = 1; i < 4; i++)
				{
					arg[i] = tok.next(cmd);
				}
				curve4(arg[0], arg[1], arg[2], arg[3], cmd == 's');
				break;

			case 'A': case 'a':
				throw exception("parse_path: Command A: NOT IMPLEMENTED YET");*/

			case 'Z': case 'z':
				{
					SVGPathCommandM * com = (SVGPathCommandM*)(curr_path_->getCommand(0));
					v2d tmp = com->move_to_pos_	;
					SVGPathCommandL * c = new SVGPathCommandL(tmp);
					curr_path_->addCommand(c);
					break;
				}

			default:
			{
				cout << cmd << " ";
				break;
				// cout<<"parse_path: Invalid Command"<<endl;
			}
		}
	}
}

void SVGParser::parse(const char * fname)
{
	XML_Parser p = XML_ParserCreate(NULL);
	if(p == 0) 
	{
		return;
	}

	XML_SetUserData(p, this);
	XML_SetElementHandler(p, startElement, endElement);
	XML_SetCharacterDataHandler(p, content);

	FILE * fd;
	fopen_s(&fd, fname, "r");
	if(fd == 0)
	{
		return;
	}

	bool done = false;
	do
	{
		size_t len = fread(buf_, 1, BUFSIZ, fd);
		done = len < BUFSIZ;
		if(!XML_Parse(p, buf_, (int)len, done))
		{
			return;
		}
	}
	while(!done);
	fclose(fd);
	XML_ParserFree(p);
}

SVGPattern * SVGParser::toSVGPattern()
{
	SVGPattern * pp = new SVGPattern();
	for(size_t i = 0;i<paths_.size();i++)
	{
		SVGElement * p = paths_[i]->pathToElement();
		pp->addElement(p);	
	}
	return pp;
}