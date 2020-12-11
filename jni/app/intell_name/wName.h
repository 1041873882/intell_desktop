
#ifndef __W_NAME_H__
#define __W_NAME_H__

#include <iostream>
#include <string>
#include <vector>
#include<set>
#include "ybh_mKeyboard.h"
#include "mWindow.h"
#include "mButton.h"
#include "mEdit.h"
using namespace std;

#define MAX_LOGGER_LINE		6

struct cmp {
    bool operator()(string const& a, string const& b) const {
        return strcasecmp(a.c_str(), b.c_str()) <= 0;
    }
};

class wName : public mWindow {
public:
	wName();
	virtual ~wName();

    struct __DATA {
        vector<string> name;
        vector<int> id;
    } data;
    set<string, cmp> name_list;
     
    void collect(int n);
    void doTimer(void);

    void load(void);
    void save(void);

	void select(int menu);
    void showData(int n);

    void search(void);
	void doEvent(mEvent *e);
    
private:
    int m_offset;
    int m_searchFirst;
    int m_first;
    int m_dataType;
    int m_num;
    int m_max;
    int m_sz;

    time_t m_ts;
    time_t m_tsTips;
    
	mPixmap m_bkg;
    mPixmap m_collect[MAX_LOGGER_LINE];
    mPixmap m_line[MAX_LOGGER_LINE+1];
    mButton m_data[MAX_LOGGER_LINE];
    mButton m_sign[26];
    mText m_name[MAX_LOGGER_LINE];
    mText m_host[MAX_LOGGER_LINE];
	mButton m_tips;
    mButton m_up;
	mButton m_down;    
    mButton m_exit;
    mEdit m_search;
};

#endif
