#include<algorithm>
#include <functional>
#include <fstream> 
#include "stdlib.h"
#include "wName.h"
#include "wMain.h"

using namespace std;

#define SYS_NAME "/dnake/data/sys_name.txt"

wName::wName() : mWindow("intell_name")
{
    int i = 0;

    m_bkg.setParent(this);
	m_bkg.load(m_style, "bkg");

    m_search.setParent(this);
    m_search.load(m_style, "search");

    m_offset = m_style.getInt("/style/offset", 70);

    while(i < MAX_LOGGER_LINE) {
        m_line[i].setParent(this);
        m_line[i].load(m_style, "line");
        m_line[i].move(m_line[0].x(), m_line[0].y()+i*m_offset);

        m_collect[i].setParent(this);
        m_collect[i].load(m_style, "collect");
        m_collect[i].move(m_collect[0].x(), m_collect[0].y()+i*m_offset);
        m_collect[i].hide();

        m_data[i].setParent(this);
        m_data[i].load(m_style, "data");
        m_data[i].move(m_data[0].x(), m_data[0].y()+i*m_offset);

        m_name[i].setParent(this);
        m_name[i].load(m_style, "name");
        m_name[i].move(m_name[0].x(), m_name[0].y()+i*m_offset);

        m_host[i].setParent(this);
        m_host[i].load(m_style, "host");
        m_host[i].move(m_host[0].x(), m_host[0].y()+i*m_offset);
        i++;
    }

    m_line[i].setParent(this);
    m_line[i].load(m_style, "line");
    m_line[i].move(m_line[0].x(), m_line[0].y()+i*m_offset);

    m_offset = m_style.getInt("/style/offset", 16);
    for (int d=0; d<26; d++) {
        char str[3];
        sprintf(str, "%c", 'A'+d);
        m_sign[d].setParent(this);
        m_sign[d].load(m_style, "sign");
        m_sign[d].setText(str);
        m_sign[d].move(m_sign[0].x(), m_sign[0].y()+d*m_offset);
    }

    m_tips.setParent(this);
    m_tips.load(m_style, "tips");
    m_tips.setEnable(0);
    m_tips.hide();

	m_up.setParent(this);
	m_up.load(m_style, "up");
	m_down.setParent(this);
	m_down.load(m_style, "down");

    m_exit.setParent(this);
	m_exit.load(m_style, "exit"); 
    
    m_sz = 0;
    m_first = 0;
    m_searchFirst = 0;
    m_dataType = 0;
    m_ts = 0;
    m_tsTips = 0;
    this->load();
    this->showData(m_first);
}

wName::~wName()
{
}

void wName::load(void)
{
    int n = 0;
    
    ifstream infile(SYS_NAME);
    string s;

    if (!infile.is_open()) {
        cout << "Error opening file" << endl;
    }

    while (getline(infile, s)) {
        name_list.insert(s);
    }

    data.name.assign(name_list.begin(), name_list.end());

    m_sz = data.name.size();
}

void wName::doEvent(mEvent *e)
{
    mWindow::doEvent(e);

    if (e->type == mEvent::TouchBegin) {
        m_ts = time(NULL);
        m_tsTips = time(NULL);
        for(int i=0; i<64; i++) {
            if (e->wParam == m_sign[i].id()) {
                m_tips.setText(m_sign[i].text());
                m_tips.show();
                break;
            }
        }
    }

    if (labs(time(NULL)-m_ts) >= 1) {
        m_ts = 0;
        int n = -1;
        for(int i=0; i<MAX_LOGGER_LINE; i++) {
            if (e->wParam == m_data[i].id() && i<m_max) {
                n = i;
                break;
            }
        }
        if (n != -1) {
            if (m_dataType)
                this->collect(n+m_searchFirst);
            else
                this->collect(n+m_first);
        }
    }

    if (e->type == mEvent::TouchEnd) {

        m_ts = 0;
        m_tsTips = time(NULL);
        this->search();

        for(int i=0; i<64; i++) {
            if (e->wParam == m_sign[i].id()) {
                m_tips.hide();
                printf("show\n");
                break;
            }
        }


        if (e->wParam == m_up.id()) {
            if (m_dataType) {
                if (m_searchFirst-MAX_LOGGER_LINE >= 0) {
                    m_searchFirst -= MAX_LOGGER_LINE;
                    this->showData(m_searchFirst);
			    }
            } else {
                if (m_first-MAX_LOGGER_LINE >= 0) {
                    m_first -= MAX_LOGGER_LINE;
                    this->showData(m_first);
			    }
            }
        } else if (e->wParam == m_down.id()) {
            int d = 0;
            if (m_dataType) {
                d = m_num;
                if (m_searchFirst+MAX_LOGGER_LINE < d) {
				    m_searchFirst += MAX_LOGGER_LINE;
				    this->showData(m_searchFirst);
			    }
            } else {
                d = m_sz;
                if (m_first+MAX_LOGGER_LINE < d) {
				    m_first += MAX_LOGGER_LINE;
				    this->showData(m_first);
			    }
            }  
        } else if (e->wParam == m_exit.id()) {
            wMain *w = new wMain();
			w->show();
        }
    }
}

void wName::collect(int n)
{
    if (n < m_sz) {
        int d;

        if (m_dataType) {
            int z = data.id[n];
            sscanf(data.name[z].c_str(), "%1d", &d);
            if (d) 
                data.name[z].replace(data.name[z].begin(), data.name[z].begin()+1, "0");
            else
                data.name[z].replace(data.name[z].begin(), data.name[z].begin()+1, "1");
        } else {
            sscanf(data.name[n].c_str(), "%1d", &d); 
            if (d) 
                data.name[n].replace(data.name[n].begin(), data.name[n].begin()+1, "0");
            else
                data.name[n].replace(data.name[n].begin(), data.name[n].begin()+1, "1");
        }
       
        set<string, cmp> name_list(data.name.begin(), data.name.end());
        data.name.assign(name_list.begin(), name_list.end());
        this->search();
    }
}

void wName::showData(int n)
{
    m_max = MAX_LOGGER_LINE;
    int x = 0;
    if (m_dataType) 
        x = m_num;
    else
        x = m_sz;
    
    if (n+MAX_LOGGER_LINE > x)   //判断下一页总共显示几条
		m_max = x-n;

    for (int i=0; i<MAX_LOGGER_LINE; i++) {
        m_collect[i].hide();
        if (i < m_max) {
            int c;
            string s1, s2;
            if (m_dataType) {
                int s = data.id[n+i];
                int d = data.name[s].rfind(" ");        //获取最后一个空格出现的位置
                sscanf(data.name[s].c_str(), "%1d", &c);
                s1 = data.name[s].substr(2, d-2);       //截取从第二个字符到指定位置的字符串
                s2 = data.name[s].substr(d);            //截取从指定位置到结尾的字符串
            } else {
                int d = data.name[n+i].rfind(" ");
                sscanf(data.name[n+i].c_str(), "%1d", &c);
                s1 = data.name[n+i].substr(2, d-2);
                s2 = data.name[n+i].substr(d);
            }

            if (c == 0)
                m_collect[i].show();

            m_name[i].setText(s1.c_str());
            m_name[i].show(); 

            m_host[i].setText(s2.c_str());
            m_host[i].show();
            m_data[i].show();    
        } else {
            m_host[i].hide();
            m_data[i].hide();
            m_name[i].hide();
            m_collect[i].hide();
        }
    }
}

void wName::search(void)
{
    if (strlen(m_search.text()) == 0) {
        m_dataType = 0;
        m_num = 0;
        this->showData(m_first);
    } else { 
        m_num = 0;

        m_dataType = 1;
        data.id.clear();
        for (int i=0; i<m_sz; i++) {
            if (strcasestr(data.name[i].c_str(), m_search.text())) {
                data.id.push_back(i);
                m_num++;
            }
        }
        this->showData(m_searchFirst);
    }
}

void wName::doTimer(void)
{
    mWindow::doTimer();
    if (labs(time(NULL)-m_tsTips) > 2) {
        m_tsTips = time(NULL);
        m_tips.hide();
    }
}
