#ifndef _H_COM_PARA_
#define _H_COM_PARA_

class CommandLinePara{
public:
    CommandLinePara(int argc,char **argv,const char *pattern);
    ~CommandLinePara();
    int getErrCode() const {
        return errCode;
    }
private:
    int errCode;
};

#endif // _H_COM_PARA_
