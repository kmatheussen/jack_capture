//-----------------------------------------------------
// name : "jack_capture_settings"
// version : "1.00"
// author : "brummer"
// license : "BSD"
// copyright : "is a present to Kjetil S. Matheussen"
//
// Prototyp generated with Faust 0.9.9.4f (http://faust.grame.fr)
//-----------------------------------------------------

/* link with  */
#include <sys/stat.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

//inline void *aligned_calloc(size_t nmemb, size_t size) { return (void*)((unsigned)(calloc((nmemb*size)+15,sizeof(char)))+15 & 0xfffffff0); }
inline void *aligned_calloc(size_t nmemb, size_t size)
{
    return (void*)((size_t)(calloc((nmemb*size)+15,sizeof(char)))+15 & ~15);
}

//	g++ -O2 `pkg-config --cflags --libs gtk+-2.0` jack_capture_settings.cpp -o giveaname

#define max(x,y) (((x)>(y)) ? (x) : (y))
#define min(x,y) (((x)<(y)) ? (x) : (y))

// ------------------define the parameter reading, take code from jack_capture -----------------------------------
#define OPTARGS_CHECK_GET(wrong,right) lokke==argc-1?(fprintf(stderr,"Must supply argument for '%s'\n",argv[lokke]),exit(-2),wrong):right
#define OPTARGS_BEGIN(das_usage) {int lokke;const char *usage=das_usage;for(lokke=1;lokke<argc;lokke++){char *a=argv[lokke];if(!strcmp("--help",a)||!strcmp("-h",a)){fprintf(stderr,usage);return 0;
#define OPTARG(name,name2) }}else if(!strcmp(name,a)||!strcmp(name2,a)){{
#define OPTARG_GETSTRING() OPTARGS_CHECK_GET("",argv[++lokke])
#define OPTARGS_END }else{fprintf(stderr,usage);return(-1);}}}

inline int		lsr (int x, int n)
{
    return int(((unsigned int)x) >> n);
}
inline int 		int2pow2 (int x)
{
    int r=0;
    while ((1<<r)<x) r++;
    return r;
}

/******************************************************************************
*******************************************************************************

								GRAPHIC USER INTERFACE (v2)
								  abstract interfaces

*******************************************************************************
*******************************************************************************/

#include <map>
#include <list>

using namespace std;

struct Meta : map<const char*, const char*>
{
    void declare (const char* key, const char* value)
    {
        (*this)[key]=value;
    }
};

struct uiItem;
typedef void (*uiCallback)(float val, void* data);

/**
 * Graphic User Interface : abstract definition
 */

class UI
{
    typedef list<uiItem*> clist;
    typedef map<float*, clist*> zmap;

private:
    static list<UI*>	fGuiList;
    zmap				fZoneMap;
    bool				fStopped;

public:

    UI() : fStopped(false)
    {
        fGuiList.push_back(this);
    }

    virtual ~UI()
    {
        // suppression de this dans fGuiList
    }

    // -- registerZone(z,c) : zone management
    void registerZone(float* z, uiItem* c)
    {
        if (fZoneMap.find(z) == fZoneMap.end()) fZoneMap[z] = new clist();
        fZoneMap[z]->push_back(c);
    }

    // -- saveState(filename) : save the value of every zone to a file
    void saveState(const char* filename)
    {
        ofstream f(filename);
        for (zmap::iterator i=fZoneMap.begin(); i!=fZoneMap.end(); i++)
        {
            f << *(i->first) << ' ';
        }
        f << endl;
        f.close();
    }

    // show the comanline given to jack_capture as tooltip when the mouse move over the record button
    //  error handler for the port argument, check if it is true
    static void showtip( GtkWidget *widget, string mand1, string mand2, string mandc)
    {
        extern const char* capturas ;
        extern GtkWidget* status;
        extern int st;
        int error = 0;
        if (mandc != "" )
        {
            string buffer,port1, port2;
            char bufer[256];
            FILE* readports;
            std::string a(mand1);
            std::string b(" --port ");
            std::string::size_type in = a.find(b);
            if ((in != -1) & (mand1 != " --port "))
            {
                a.replace(in, 8, "");
                a += "\n|";
            }
            else a = "no";
            std::string d(mand2);
            in = d.find(b);
            if ((in != -1) & (mand2 != " --port "))
            {
                d.replace(in, 8, "");
                d += "\n|";
            }
            else d = "no";
            readports = popen("jack_lsp", "r");
            while (fgets (bufer, 256, readports) != NULL)
            {
                b = bufer;
                b += "|";
                in = b.find(a);
                if ((in == 0) & (a.c_str() != "no"))
                {
                    error += 1;
                }
                in = b.find(d);
                if ((in == 0) & (d.c_str() != "no"))
                {
                    error += 1;
                }
            }
            pclose(readports);
            if (mandc == " -c 1" ) error += 1;
        }
        else error = 2;

        if (error == 2)
        {
            gtk_widget_set_sensitive(widget, TRUE);
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, capturas, "the comandline to jack_capture.");
            st = gtk_statusbar_push(GTK_STATUSBAR(status), st, "Ready");
        }
        else if (error == 0)
        {
            gtk_widget_set_sensitive(widget, FALSE);
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, "ERROR\nis attainable the choose'n record Port  ?", "the comandline to jack_capture.");
            st = gtk_statusbar_push(GTK_STATUSBAR(status), st, "ERROR");
        }
#if 0
        if (system(" pidof jackd.bin >/dev/null") != 0)
        {
            gtk_widget_set_sensitive(widget, FALSE);
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, "ERROR\njack server not running", "the comandline to jack_capture.");
            st = gtk_statusbar_push(GTK_STATUSBAR(status), st, "ERROR");
        }
#endif
    }

    // convert int to string
    void IntToString(int i, string & s)
    {
        s = "";
        if (i == 0)
        {
            s = "0";
            return;
        }
        if (i < 0)
        {
            s += '-';
            i = -i;
        }
        int count = log10(i);
        while (count >= 0)
        {
            s += ('0' + i/pow(10.0, count));
            i -= static_cast<int>(i/pow(10.0,count)) * static_cast<int>(pow(10.0,count));
            count--;
        }
    }

    // -- readactuellState(filename) : read the value of every zone to a string
    void readactuellState(const char* fname, const char* path,  const char* input,  const char* output, const char* serverpath, const char* sett)
    {
        extern const char* capturas ;
        extern GtkWidget* 	recbutton;
        extern string com;
        extern int getset;
        extern int countfile;
        extern gchar* text;
        extern gchar* text1;

        int is;
        int checkit = 1;
        const char* oggit = " ";
        const char* lameit = " ";
        int sel = 0;
        string  mand, mandi, mando, manda, mandc, mandm, mandme, mand1, mand2, ma, can, can1, com2;
        float nobitdepth = 0;

        if (strcmp(serverpath, " ") == 0)
        {
            com = "jack_capture";
        }
        else
        {
            com = serverpath;
        }
        // first is the meterbridge
        zmap::iterator i=fZoneMap.begin();
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            mandme = " -mt dpm"; // set meterbridge to sco
        }
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            if (strcmp(fname, " ") == 0)
            {
                mandi = " -fp ~/session";
            }
            else
            {
                IntToString(countfile,mand);
                std::string a(fname);
                std::string b(".");
                std::string::size_type in = a.find(b);
                if (in != -1) a.erase(in);
                mandi = " ";
                mandi += a;
                mandi += mand;
                mandi += ".wav";
            }
            nobitdepth = 0;
        }
        // read it is ogg file
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            if (strcmp(fname, " ") == 0)
            {
              mandi = " -f ogg -fp ~/session ";
            }
            else
            {
                mandi = " -f ogg -fn ";
                IntToString(countfile,mand);
                std::string a(fname);
                std::string b(".");
                std::string::size_type in = a.find(b);
                if (in != -1) a.erase(in);
                mandi += a;
                mandi += mand;
                mandi += ".ogg";
            }
            // set float as default format for ogg set it to 0 to disable that
            nobitdepth = 1;
            oggit = "y";
        }
        // read it is mp3 file
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            if (strcmp(fname, " ") == 0)
            {
                mandi = " -ws|lame -V2 -r -  ~/session";
                {
                  char base_filename[5000];
                  for(;;){
                    sprintf(base_filename,"%s/session%d.mp3",getenv("HOME"),countfile);
                    fprintf(stderr,"base: -%s-\n",base_filename);
                    if(access(base_filename,F_OK)) break;
                    countfile++;
                  }
                }
                IntToString(countfile,mand);
                mandi += mand;
                mandi += ".mp3";
            }
            else
            {
                mandi = " -ws|lame -V2 -r - ";
                IntToString(countfile,mand);
                std::string a(fname);
                std::string b(".");
                std::string::size_type in = a.find(b);
                if (in != -1) a.erase(in);
                mandi += a;
                mandi += mand;
                mandi += ".mp3";
            }
            // set float as default format for mp3
            nobitdepth = 1;
            lameit = "y";
        }
        // read it is flac file
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            if (strcmp(fname, " ") == 0)
            {
                mandi = " -f flac  -fp ~/session ";
            }
            else
            {
                mandi = " -f flac ";
                IntToString(countfile,mand);
                std::string a(fname);
                std::string b(".");
                std::string::size_type in = a.find(b);
                if (in != -1) a.erase(in);
                mandi += a;
                mandi += mand;
                mandi += ".flac";
            }
            // set float as default format for flac also.
            nobitdepth = 1;
        }
        // read bitdepth it is float
        is =  *(i->first);
        i++;
        if ((is == 1) & (nobitdepth == 0 ))
        {
            // nothing to do when float is selected
        }
        // read bitdepth it is 32
        is =  *(i->first);
        i++;
        if ((is == 1) & (nobitdepth == 0 ))
        {
            mand = " -b 32";
            com += mand;
        }
        // read bitdepth it is 24
        is =  *(i->first);
        i++;
        if ((is == 1) & (nobitdepth == 0 ))
        {
            mand = " -b 24";
            com += mand;
        }
        // read bitdepth it is 16
        is =  *(i->first);
        i++;
        if ((is == 1) & (nobitdepth == 0 ))
        {
            mand = " -b 16";
            com += mand;
        }
        // read bitdepth it is 8
        is =  *(i->first);
        i++;
        if ((is == 1) & (nobitdepth == 0 ))
        {
            mand = " -b 8";
            com += mand;
        }
        // read port it is output
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            mand = " --port ";
            mand += output;
            checkit = 0;
        }
        // read port it is input
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            mand = " --port ";
            mand += input;
            checkit = 0;
        }
        // read port it is playback
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            // port selected from combolist
            sel = 1;
            if (text != NULL)
            {
                mand = " --port ";
                std::string a(text);
                mand += a;
                mand1 = mand;
            }
        }
        // read enable db_meter
        is =  *(i->first);
        i++;
        if (is == 0)
        {
            mandm += " --silent --disable-meter";
        }
        else mandm += " -lm";
        // read 2 channels
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            mandc = " -c 2";
            com += mandc;
            // second port from combolist
            if (sel == 1)
            {
                if (text1 != NULL)
                {
                    std::string b(text1);
                    mand2 = " --port ";
                    mand2 += b;
                    mand += mand2;
                    sel = 0;
                    //   fprintf(stderr, "%s\n", text1);
                }
            }
        }
        // read 1 channel
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            mandc = " -c 1";
            com += mandc;
            // case input file is 1 channel set output file also to 1 channel for ogg and mp3
            if (lameit == "y")
            {
                if (strcmp(fname, " ") == 0)
                {
                    mandi = " -ws|lame -m m -r -  ~/session";
                    {
                      char base_filename[5000];
                      for(;;){
                        sprintf(base_filename,"%s/session%d.mp3",getenv("HOME"),countfile);
                        fprintf(stderr,"base: -%s-\n",base_filename);
                        if(access(base_filename,F_OK)) break;
                        countfile++;
                      }
                    }
                    IntToString(countfile,ma);
                    mandi += ma;
                    mandi += ".mp3";
                }
                else
                {
                    mandi = " -ws|lame -m m -r - ";
                    IntToString(countfile,ma);
                    std::string a(fname);
                    std::string b(".");
                    std::string::size_type in = a.find(b);
                    if (in != -1) a.erase(in);
                    mandi += a;
                    mandi += ma;
                    mandi += ".mp3";
                }
            }
            //}
        }
        // read set channel auto
        is =  *(i->first);
        i++;
        if (is == 1)
        {
            if (sel == 1)
            {
                // second port from combolist
                if (text1 != NULL)
                {
                    std::string b(text1);
                    mand2 = " --port ";
                    mand2 += b;
                    mand += mand2;
                    sel = 0;
                }
            }
            // nothing to do for auto selected channel setting
        }
        // write all in string com and to const char capturas
        com += mandme;
        com += mandm;
        com += mand;
        com += mandi;
        capturas = com.c_str();
        getset = 1;
        //  actuell setings to showtip
        if (strcmp(sett, "yes") != 0)
        {
            if (checkit != 1) mandc = "";
            showtip(recbutton, mand1, mand2, mandc);
        }
    }

    // -- recallState(filename) : load the value of every zone from a file
    void recallState(const char* filename)
    {
        ifstream f(filename);
        if (f.good())
        {
            for (zmap::iterator i=fZoneMap.begin(); i!=fZoneMap.end(); i++)
            {
                f >> *(i->first);
            }
        }
        f.close();
    }

    void updateAllZones();

    void updateZone(float* z);

    static void updateAllGuis()
    {
        list<UI*>::iterator g;
        for (g = fGuiList.begin(); g != fGuiList.end(); g++)
        {
            (*g)->updateAllZones();
        }
    }

    // -- active widgets
    virtual void addComboBox(int i, const char* label, float* zone) {};
    virtual void addButton(const char* label, float* zone) {};
    virtual void addExitButton(const char* label, float* zone) {};
    virtual void addRecToggleButton(const char* label, float* zone) {};
    virtual void addCheckButton(const char* label, float* zone) {};
    virtual void addRadioButton(GtkWidget* group, const char* label, float* zone) {};
    virtual void addRadioButtonwithGroup(const char* label, float* zone) {};

    void addCallback(float* zone, uiCallback foo, void* data);

    // -- widget's layouts
    virtual void addDrawBox(int i) {};
    virtual void openHorizontalBox(const char* label) {};
    virtual void openVerticalBox(const char* label) {};
    virtual void openEventBox(int i, const char* label) {};
    virtual void openExpanderBox(const char* label, float* zone) {};
    virtual void closeBox() {};

    virtual void show() {};
    virtual void run() {};

    void stop()
    {
        fStopped = true;
    }
    bool stopped()
    {
        return fStopped;
    }

    virtual void declare(float* zone, const char* key, const char* value) {}
};


/**
 * User Interface Item: abstract definition
 */

class uiItem

{
protected :

    UI*		fGUI;
    float*		fZone;
    float		fCache;

    uiItem (UI* ui, float* zone) : fGUI(ui), fZone(zone), fCache(-123456.654321)
    {
        ui->registerZone(zone, this);
    }


public :
    virtual ~uiItem() {}



    void modifyZone(float v)
    {
//-------------- this are the parameter used from argv[] ---------------------------
        extern const char*			path;
        extern const char*			serverpath;
        extern const char*			fname;
        extern const char*			sett;
        extern const char* 			input;
        extern const char* 			output;
        fCache = v;
        if (*fZone != v)
        {
            *fZone = v;
            fGUI->updateZone(fZone);
            // translate GUI state to comandline
            fGUI->readactuellState(fname, path, input, output, serverpath, sett);
        }

    }

    float			cache()
    {
        return fCache;
    }
    virtual void 	reflectZone() 	= 0;
};


/**
 * Callback Item
 */

struct uiCallbackItem : public uiItem
{
    uiCallback	fCallback;
    void*		fData;

    uiCallbackItem(UI* ui, float* zone, uiCallback foo, void* data)
            : uiItem(ui, zone), fCallback(foo), fData(data) {}

    virtual void 	reflectZone()
    {
        float 	v = *fZone;
        fCache = v;
        fCallback(v, fData);
    }
};

// en cours d'installation de call back. a finir!!!!!

/**
 * Update all user items reflecting zone z
 */

inline void UI::updateZone(float* z)
{
    float 	v = *z;
    clist* 	l = fZoneMap[z];
    for (clist::iterator c = l->begin(); c != l->end(); c++)
    {
        if ((*c)->cache() != v) (*c)->reflectZone();
    }
}


/**
 * Update all user items not up to date
 */

inline void UI::updateAllZones()
{
    for (zmap::iterator m = fZoneMap.begin(); m != fZoneMap.end(); m++)
    {
        float* 	z = m->first;
        clist*	l = m->second;
        float	v = *z;
        for (clist::iterator c = l->begin(); c != l->end(); c++)
        {
            if ((*c)->cache() != v) (*c)->reflectZone();
        }
    }
}

inline void UI::addCallback(float* zone, uiCallback foo, void* data)
{
    new uiCallbackItem(this, zone, foo, data);
};


/******************************************************************************
*******************************************************************************

								GRAPHIC USER INTERFACE
								  gtk interface

*******************************************************************************
*******************************************************************************/

#include <gtk/gtk.h>
#define stackSize 256

// Insertion modes

#define kSingleMode 0
#define kBoxMode 1
#define kTabMode 2

class GTKUI : public UI
{
private :
    static bool			fInitialized;
    static list<UI*>	fGuiList;

protected :
    GtkWidget* 	fWindow;
    int			fTop;
    GtkWidget* 	fBox[stackSize];
    int 		fMode[stackSize];
    bool		fStopped;

    GtkWidget* addWidget(const char* label, GtkWidget* w);
    virtual void pushBox(int mode, GtkWidget* w);


public :

    static const gboolean expand = TRUE;
    static const gboolean fill = TRUE;
    static const gboolean homogene = FALSE;

    GTKUI(char * name, int* pargc, char*** pargv);

    // -- layout groups
    virtual void addDrawBox(int i);
    virtual void openHorizontalBox(const char* label = "");
    virtual void openVerticalBox(const char* label = "");
    virtual void openEventBox(int i, const char* label = "");
    virtual void openExpanderBox(const char* label, float* zone);
    virtual void closeBox();

    // -- active widgets
    virtual void addComboBox(int i, const char* label, float* zone);
    virtual void addButton(const char* label, float* zone);
    virtual void addExitButton(const char* label, float* zone);
    virtual void addRecToggleButton(const char* label, float* zone);
    virtual void addCheckButton(const char* label, float* zone);
    virtual void addRadioButton(GtkWidget* group, const char* label, float* zone);
    virtual void addRadioButtonwithGroup(const char* label, float* zone);

    virtual void show();
    virtual void run();

};

/******************************************************************************
*******************************************************************************

								GRAPHIC USER INTERFACE (v2)
								  gtk implementation

*******************************************************************************
*******************************************************************************/

// global static fields
FILE*               control_stream;



//-------------- check if the save path exists and create first settings-------------------
bool Exists()
{
    const char*	  home;
    char                dirname[256];
    char                rcfilename[256];
    home = getenv ("HOME");
    if (home == 0) home = ".";
    snprintf(dirname, 256, "%s/.%s", home, "jack_capture");
    struct stat my_stat;
    if  ( !stat(dirname, &my_stat) == 0)
    {
        system("mkdir $HOME/.jack_capture" );
        snprintf(rcfilename, 256, "%s/.%src", home, "jack_capture/settings");
        ofstream f(rcfilename);
        string com = "0 1 0 0 0 1 0 0 0 0 1 0 0 0 0 0 1 0 1 0  ";
        f <<  com <<endl;
        f.close();
    }
}

bool		GTKUI::fInitialized = false;
list<UI*>	UI::fGuiList;
GtkWidget* 	status;
GtkWidget* 	buttongroup;
GtkWidget* 	recbutton;
GtkWidget* 	combo;
GtkWidget* 	combo1;
GtkWidget* 	button1 ;
//--------- global parameters -----------
UI*                 interface;
float 	rec3 = 0;
const char* input;
const char* output;
const char* capturas;
gchar* text;
gchar* text1;
string in, out;
float settip = 0;
int getset = 0;
int countfile = 0;
int st;
int intext = 0;
string com;  // this is the comandline string
//-------------- this are the parameter used from argv[] ---------------------------
const char*			path = " " ;
const char*			serverpath = " " ;
const char*			appname = " " ;
const char*			sett = " " ;
const char*			fname = " " ;

static gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data )
{
    return FALSE;
}

//----------------- read active entry in combobox selected port -------------
static void restore_plug( GtkWidget *widget, gpointer data )
{
    text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
    text1 = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo1));
    interface->readactuellState(fname, path, input, output, serverpath, sett);
}

//----------------- reload  entrys in combobox selected port -------------
static void delet_plug( GtkWidget *widget, gpointer data )
{
    gint v = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
    gint v1 = gtk_combo_box_get_active(GTK_COMBO_BOX(combo1));
    for (int i = 0; i < intext; i++)
    {
        gtk_combo_box_remove_text (GTK_COMBO_BOX(combo), 0 );
        gtk_combo_box_remove_text (GTK_COMBO_BOX(combo1), 0 );
    }
    intext = 0;
    char bufer[100];
    char bufer1[100];
    FILE* readports;
    readports = popen("jack_lsp -t", "r");
    while (fgets (bufer, 100, readports) != NULL)
    {
        std::string e(bufer);
        std::string a = "\n";
        std::string::size_type isn = e.find(a);
        if (isn != -1) e.erase(isn);
        a = "audio";
        isn = e.find(a);
        if ((isn == -1) ) snprintf(bufer1, 100, e.c_str());
        if ((isn != -1) )
        {
            gtk_combo_box_append_text(GTK_COMBO_BOX(combo1), bufer1);
            gtk_combo_box_append_text(GTK_COMBO_BOX(combo), bufer1);
            intext++;
        }
    }
    pclose(readports);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), v);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo1), v1);
}

//---------------------- quit gui  and close jack_capture in the case it is still runing--------------------------------
static void destroy_event( GtkWidget *widget, gpointer data )
{
    if (system(" ps xa | grep -v grep | grep \"jack_capture \" > /dev/null") == 0)
    {
        fputs("quit\n",control_stream);
        fflush(control_stream);
        //pclose(control_stream);
	sleep(2);
	system(" killall jack_capture  2> /dev/null");
    }
    gtk_main_quit ();
}

// -- readState(filename) : read the comandline for jack_capture from the gui state and write to a file
void readState(const char* path)
{
    const char* home;
    char gfilename[256];
    home = getenv ("HOME");
    if (home == 0) home = ".";
    if (strcmp(path, " ") == 0)
    {
        snprintf(gfilename, 256, "%s%src", home, "/.jack_capture/ja_ca_set");
    }
    else
    {
        snprintf(gfilename, 256, "%s%s", home, path);
    }
//----------------------------- save the results to a file ---------------------------------------
    ofstream fa(gfilename);
    fa <<  com <<endl;
    fa.close();
}

// show the comanline given to jack_capture as tooltip when the mouse move over the record button
static void show_tip( GtkWidget *widget)
{
// read comandline from file
    if (getset == 0)
    {
        string buf;
        const char* home;
        char gfilename[256];
        home = getenv ("HOME");
        if (home == 0) home = ".";
        if (strcmp(path, " ") == 0)
        {
            snprintf(gfilename, 256, "%s%src", home, "/.jack_capture/ja_ca_set");
        }
        else
        {
            snprintf(gfilename, 256, "%s%s", home, path);
        }
        if (settip == 0)
        {
            ifstream f(gfilename);
            if (f.good())
            {
                getline(f, buf);
                capturas = buf.c_str();
                f.close();
            }
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, capturas, "the comandline to jack_capture.");
        }
        else if (settip == 1)
        {
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, "RECORDING IN PROGRESS", "jack_capture run");
            settip = 0;
        }
        else if (settip == 2)
        {
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, "RECORDING ERROR", "jack_capture run");
            settip = 0;
        }
    }
    // read comandline from string
    else
    {
        if (settip == 0)
        {
            interface->readactuellState(fname, path, input, output, serverpath, sett);
            capturas = com.c_str();
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, capturas, "the comandline to jack_capture.");
        }
        else if (settip == 1)
        {
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, "RECORDING IN PROGRESS", "jack_capture run");
            settip = 0;
        }
        else if (settip == 2)
        {
            GtkTooltips *comandline = gtk_tooltips_new ();
            gtk_tooltips_set_tip (GTK_TOOLTIPS (comandline), widget, "ERROR\nin record?", "jack_capture run");
            settip = 0;
        }
    }
}

//---------------- record run jack_capture with popen() capturas is the comandline read from the file
bool capture(const char* capturas)
{
    countfile = countfile + 1; // count the recorded files
    fprintf(stderr, "%s\n", capturas);  // print out the comanline to the terminal
    control_stream = popen (capturas, "w");
    settip = 1;
    getset = 1;
    st = gtk_statusbar_push(GTK_STATUSBAR(status), st, "RECORD");
    // minimal error handling
    if (system("sleep 1\n ps xa | grep -v grep | grep \"jack_capture \"  > /dev/null") != 0)
    {
        st = gtk_statusbar_push(GTK_STATUSBAR(status), st, "ERROR");
        fprintf(stderr, "error\n");
        GdkColor colorRed;
        GdkColor colorOwn;
        gdk_color_parse ("#edef1e", &colorOwn);
        gdk_color_parse ("#fafb83", &colorRed);
        gtk_widget_modify_bg (recbutton, GTK_STATE_PRELIGHT, &colorOwn);
        gtk_widget_modify_bg (recbutton, GTK_STATE_ACTIVE, &colorRed);
        settip = 2;
    }
    show_tip(recbutton);
}

//-------------- set the name for the channels to lounch given by argv[] appname ----------------
static void set_appname()
{
    if (strcmp(appname, " ") != 0)
    {
        in = appname;
        in += ":in*";
        input = in.c_str();
        out = appname;
        out += ":out*";
        output = out.c_str();
    }
    else
    {
        input = "system:capture*";
        output = "system:playback*";
    }
}

//--------------------- save the gui settings as commandline to file ---------------------------
static void save_event( GtkWidget *widget, gpointer data )
{
    char                rcfilename[256];
    const char*			home;
    home = getenv ("HOME");
    if (home == 0) home = ".";
    snprintf(rcfilename, 256, "%s/.%src", home, "jack_capture/settings");
    interface->saveState(rcfilename);
    readState( path);
    if (strcmp(sett, "yes") != 0)
    {
        show_tip(recbutton);
    }
    if (strcmp(sett, "yes") == 0)
    {
        gtk_main_quit ();
    }
}

//---------------- funktions for the recordbutton --------------------------------------------
static void recordit( GtkWidget *widget, gpointer data )
{
//-------- end record by press the recordbutton the second time
    if (rec3 == 1)
    {
        fprintf (stderr, "stop\n");
        rec3 = 0;
        if (system(" ps xa | grep -v grep | grep \"jack_capture \" > /dev/null") == 0)
        {
            fputs("\n",control_stream);
            fflush(control_stream);
            //pclose(control_stream);
        }
        else
        {
            GdkColor colorRed;
            GdkColor colorOwn;
            gdk_color_parse ("#fba6a6", &colorOwn);
            gdk_color_parse ("#f48784", &colorRed);
            gtk_widget_modify_bg (recbutton, GTK_STATE_PRELIGHT, &colorOwn);
            gtk_widget_modify_bg (recbutton, GTK_STATE_ACTIVE, &colorRed);
        }
        st = gtk_statusbar_push(GTK_STATUSBAR(status), st, "Ready");
        show_tip(recbutton);
    }
//-------- start record by press the recordbutton the first time
    else if (rec3 == 0)
    {
        fprintf (stderr, "record\n");

        if (getset == 0)
        {
            string buf;
            // get filename
            const char* home;
            char gfilename[256];
            home = getenv ("HOME");
            if (home == 0) home = ".";
            if (strcmp(path, " ") == 0)
            {
                snprintf(gfilename, 256, "%s%src", home, "/.jack_capture/ja_ca_set");
            }
            else
            {
                snprintf(gfilename, 256, "%s%s", home, path);
            }
            // open file and read the line into const char* capturas
            ifstream f(gfilename);
            if (f.good())
            {
                getline(f, buf);
                capturas = buf.c_str();
                f.close();
                capture(capturas);
            }
        }
        // read comandline from string
        else
        {
            capturas = com.c_str();
            capture(capturas);
        }
        rec3 = 1;
    }
}

GTKUI::GTKUI(char * name, int* pargc, char*** pargv)
{
    //------- check if save path exists ----------------
    Exists();
    if (!fInitialized)
    {
        gtk_init(pargc, pargv);
        fInitialized = true;
    }
    //-------- create main window
    fWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    // modify colors mainwindow
    GdkColor colorOwn;
    gdk_color_parse ("#a8acae", &colorOwn);
    gtk_widget_modify_bg (GTK_WIDGET(fWindow), GTK_STATE_NORMAL, &colorOwn);
    // gtk_window_set_position (GTK_WINDOW(fWindow), GTK_WIN_POS_MOUSE);
    //gtk_window_set_icon_from_file  (GTK_WINDOW(fWindow), "jack_capture.png", NULL);
    //gtk_rc_parse("jack_capture.rc");
    status = gtk_statusbar_new ();
    st = gtk_statusbar_get_context_id (GTK_STATUSBAR(status), "");
    st = gtk_statusbar_push(GTK_STATUSBAR(status), st, "Ready");
    gtk_window_set_resizable(GTK_WINDOW (fWindow) , FALSE);
    gtk_window_set_title (GTK_WINDOW (fWindow), "jack_capture settings");
    gtk_signal_connect (GTK_OBJECT (fWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
    gtk_signal_connect (GTK_OBJECT (fWindow), "destroy", GTK_SIGNAL_FUNC (destroy_event), NULL);
    fTop = 0;
    fBox[fTop] = gtk_vbox_new (homogene, 4);
    fMode[fTop] = kBoxMode;
    gtk_box_pack_end (GTK_BOX ( fBox[fTop]), status, TRUE, TRUE, 0);
    gtk_container_add (GTK_CONTAINER (fWindow), fBox[fTop]);
    gtk_widget_show(status);
    fStopped = false;
}

// empilement des boites

void GTKUI::pushBox(int mode, GtkWidget* w)
{
    assert(++fTop < stackSize);
    fMode[fTop] 	= mode;
    fBox[fTop] 		= w;
}

void GTKUI::closeBox()
{
    assert(--fTop >= 0);
}

// les differentes boites

void GTKUI::addDrawBox(int i)
{
    FILE *readports;
    char bufer[256];
    const char* b;
    string a;
    if (strcmp(serverpath, " ") == 0)
    {
        a = "jack_capture -v";
    }
    else
    {
        a = serverpath;
        a += " -v 2>/dev/null";
    }
    b = a.c_str();
    readports = popen(b, "r");
    fgets (bufer, 256, readports);
    pclose(readports);
    a = bufer;
    std::string e =  "\n";
    std::string::size_type isn = a.find(e);
    if (isn != -1)
    {
        a.erase(isn);
    }
    else a = "not found ?";
    snprintf(bufer, 256, "%s%s", " jack_capture \n  v. ", a.c_str());
    GtkWidget* label = gtk_label_new (bufer);
    GdkColor colorOwn;
    gdk_color_parse ("#c1c1c8", &colorOwn);
    gtk_widget_modify_fg (label, GTK_STATE_NORMAL, &colorOwn);
    gtk_container_add (GTK_CONTAINER(fBox[fTop]), label);
    gtk_widget_show(label);
}

void GTKUI::openEventBox(int i, const char* label)
{
    GtkWidget * box = gtk_hbox_new (homogene, 0);
    gtk_container_set_border_width (GTK_CONTAINER (box), 0);
    if (fMode[fTop] != kTabMode && label[0] != 0)
    {
        GtkWidget * frame = addWidget(label, gtk_event_box_new ());

        if (i == 1)
        {
            GdkColor colorOwn;
            gdk_color_parse ("#c1c1c8", &colorOwn);
            gtk_widget_modify_bg (frame, GTK_STATE_NORMAL, &colorOwn);
        }
        else if (i == 2)
        {
            GdkColor colorOwn;
            gdk_color_parse ("#acacb2", &colorOwn);
            gtk_widget_modify_bg (frame, GTK_STATE_NORMAL, &colorOwn);
        }
        else if (i == 3)
        {
            GdkColor colorOwn;
            gdk_color_parse ("#8f969a", &colorOwn);
            gtk_widget_modify_bg (frame, GTK_STATE_NORMAL, &colorOwn);
        }
        else if (i == 4)
        {
            GdkColor colorOwn;
            gdk_color_parse ("#555555", &colorOwn);
            gtk_widget_modify_bg (frame, GTK_STATE_NORMAL, &colorOwn);
        }
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    }
    else
    {
        pushBox(kBoxMode, addWidget(label, box));
    }
}

//--------------- expander box -------------------------------
struct uiExpanderBox : public uiItem
{
    GtkExpander* fButton;
    uiExpanderBox(UI* ui, float* zone, GtkExpander* b) : uiItem(ui, zone), fButton(b) {}
    static void expanded (GtkWidget *widget, gpointer data)
    {
        float	v = gtk_expander_get_expanded  (GTK_EXPANDER(widget));
        if (v == 1.000000)
        {
            v = 0;
        }
        else v = 1;
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()
    {
        float 	v = *fZone;
        fCache = v;
        gtk_expander_set_expanded(GTK_EXPANDER(fButton), v);
    }
};

void GTKUI::openExpanderBox(const char* label, float* zone)
{
    *zone = 0.0;
    GtkWidget * box = gtk_vbox_new (homogene, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box), 4);
    if (fMode[fTop] != kTabMode && label[0] != 0)
    {
        GtkWidget * frame = addWidget(label, gtk_expander_new_with_mnemonic (label));
        GdkColor colorOwn;
        gdk_color_parse ("#c1c6c8", &colorOwn);
        gtk_widget_modify_bg (frame, GTK_STATE_PRELIGHT, &colorOwn);
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
        uiExpanderBox* c = new uiExpanderBox(this, zone, GTK_EXPANDER(frame));
        gtk_signal_connect (GTK_OBJECT (frame), "activate", GTK_SIGNAL_FUNC (uiExpanderBox::expanded), (gpointer) c);
    }
    else
    {
        pushBox(kBoxMode, addWidget(label, box));
    }

}

void GTKUI::openHorizontalBox(const char* label)
{
    GtkWidget * box = gtk_hbox_new (homogene, 4);
    if (fMode[fTop] != kTabMode && label[0] != 0)
    {
        gtk_container_set_border_width (GTK_CONTAINER (box), 2);
        GtkWidget * frame = addWidget(label, gtk_frame_new (label));
        gtk_frame_set_label_align(GTK_FRAME(frame), 0.5, 0.0);
        GdkColor colorOwn;
        gdk_color_parse ("#002200", &colorOwn);
        gtk_widget_modify_fg (frame, GTK_STATE_NORMAL, &colorOwn);
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    }
    else
    {
        gtk_container_set_border_width (GTK_CONTAINER (box), 0);
        GtkWidget * frame = addWidget(NULL, gtk_frame_new (NULL));
        gtk_frame_set_label_align(GTK_FRAME(frame), 0.5, 0.0);
        GdkColor colorOwn;
        gdk_color_parse ("#000022", &colorOwn);
        gtk_widget_modify_fg (frame, GTK_STATE_NORMAL, &colorOwn);
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    }
}

void GTKUI::openVerticalBox(const char* label)
{
    GtkWidget * box = gtk_vbox_new (homogene, 4);
    if (fMode[fTop] != kTabMode && label[0] != 0)
    {
        gtk_container_set_border_width (GTK_CONTAINER (box), 2);
        GtkWidget * frame = addWidget(label, gtk_frame_new (label));
        gtk_frame_set_label_align(GTK_FRAME(frame), 0.5, 0.0);
        GdkColor colorOwn;
        gdk_color_parse ("#002200", &colorOwn);
        gtk_widget_modify_fg (frame, GTK_STATE_NORMAL, &colorOwn);
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    }
    else
    {
        gtk_container_set_border_width (GTK_CONTAINER (box), 0);
        GtkWidget * frame = addWidget(NULL, gtk_frame_new (NULL));
        gtk_frame_set_label_align(GTK_FRAME(frame), 0.5, 0.0);
        GdkColor colorOwn;
        gdk_color_parse ("#000022", &colorOwn);
        gtk_widget_modify_fg (frame, GTK_STATE_NORMAL, &colorOwn);
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    }
}

GtkWidget* GTKUI::addWidget(const char* label, GtkWidget* w)
{
    switch (fMode[fTop])
    {
    case kSingleMode	:
        gtk_container_add (GTK_CONTAINER(fBox[fTop]), w);
        break;
    case kBoxMode 		:
        gtk_box_pack_start (GTK_BOX(fBox[fTop]), w, expand, fill, 0);
        break;
    case kTabMode 		:
        gtk_notebook_append_page (GTK_NOTEBOOK(fBox[fTop]), w, gtk_label_new_with_mnemonic(label));
        break;
    }
    gtk_widget_show (w);
    return w;
}

// ---------------------------	Combo Box ---------------------------

struct uiComboBox : public uiItem
{
    GtkComboBox* fButton;
    uiComboBox(UI* ui, float* zone, GtkComboBox* b) : uiItem(ui, zone), fButton(b) {}
    static void changed (GtkWidget *widget, gpointer data)
    {
        float	v = gtk_combo_box_get_active  (GTK_COMBO_BOX(widget));
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()
    {
        float 	v = *fZone;
        fCache = v;
        if (v == -1.000000) v = 0;
        gtk_combo_box_set_active(GTK_COMBO_BOX(fButton), v);
    }
};

void GTKUI::addComboBox(int i, const char* label, float* zone)
{
    if (i == 0)
    {
        *zone = 0.0;
        combo = gtk_combo_box_new_text();
        gtk_rc_parse_string ("style \"background\"\n"
                             "{\n"
                             "bg[NORMAL] = \"#c1c1c8\"\n"
                             "bg[ACTIVE] = \"#c1c6c8\"\n"
                             "bg[PRELIGHT] = \"#c1c6c8\"\n"
                             "}"
                             "widget_class \"*GtkCombo*\" style \"background\""
                             "widget_class \"*GtkMenu*\" style \"background\""
                            );
        addWidget(label, combo);
        FILE *readports;
        char bufer[100];
        char bufer1[100];
        string a;
        readports = popen("jack_lsp -t", "r");
        while (fgets (bufer, 100, readports) != NULL)
        {
            std::string e(bufer);
            a = "\n";
            std::string::size_type isn = e.find(a);
            if (isn != -1) e.erase(isn);
            a = "audio";
            isn = e.find(a);
            if ((isn == -1) ) snprintf(bufer1, 100, e.c_str());
            if ((isn != -1) )
            {
                gtk_combo_box_append_text(GTK_COMBO_BOX(combo), bufer1);
                intext++;
            }
        }
        pclose(readports);
        uiComboBox* c = new uiComboBox(this, zone, GTK_COMBO_BOX(combo));
        gtk_signal_connect (GTK_OBJECT (combo), "changed", GTK_SIGNAL_FUNC (uiComboBox::changed), (gpointer) c);
        gtk_signal_connect (GTK_OBJECT (combo), "changed", GTK_SIGNAL_FUNC (restore_plug), (gpointer) combo);
    }
    else if (i == 1)
    {
        *zone = 0.0;
        combo1 = gtk_combo_box_new_text();
        addWidget(label, combo1);
        FILE *readports;
        char bufer[100];
        char bufer1[100];
        string a;
        readports = popen("jack_lsp -t", "r");
        while (fgets (bufer, 100, readports) != NULL)
        {
            std::string e(bufer);
            a = "\n";
            std::string::size_type isn = e.find(a);
            if (isn != -1) e.erase(isn);
            a = "audio";
            isn = e.find(a);
            if ((isn == -1) ) snprintf(bufer1, 100, e.c_str());
            if ((isn != -1) )
            {
                gtk_combo_box_append_text(GTK_COMBO_BOX(combo1), bufer1);
            }
        }
        pclose(readports);
        uiComboBox* c = new uiComboBox(this, zone, GTK_COMBO_BOX(combo1));
        gtk_signal_connect (GTK_OBJECT (combo1), "changed", GTK_SIGNAL_FUNC (uiComboBox::changed), (gpointer) c);
        gtk_signal_connect (GTK_OBJECT (combo1), "changed", GTK_SIGNAL_FUNC (restore_plug), (gpointer) combo1);
    }
}

// --------------------------- Press button ---------------------------

struct uiButton : public uiItem
{
    GtkButton* 	fButton;
    uiButton (UI* ui, float* zone, GtkButton* b) : uiItem(ui, zone), fButton(b) {}
    static void pressed( GtkWidget *widget, gpointer   data )
    {
        uiItem* c = (uiItem*) data;
        c->modifyZone(1.0);
    }

    static void released( GtkWidget *widget, gpointer   data )
    {
        uiItem* c = (uiItem*) data;
        c->modifyZone(0.0);
    }

    virtual void reflectZone()
    {
        float 	v = *fZone;
        fCache = v;
        if (v > 0.0) gtk_button_pressed(fButton);
        else gtk_button_released(fButton);
    }
};

void GTKUI::addButton(const char* label, float* zone)
{
    *zone = 0.0;
    GtkWidget* 	button = gtk_button_new_with_mnemonic (label);
    GtkWidget * box = gtk_hbox_new (homogene, 4);
    GtkWidget * box1 = gtk_vbox_new (homogene, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box), 0);
    gtk_container_set_border_width (GTK_CONTAINER (box1), 0);
    gtk_container_add (GTK_CONTAINER(box), box1);
    gtk_container_add (GTK_CONTAINER(box), button);
    gtk_widget_set_size_request (GTK_WIDGET(box1), 72.0, 22.0);
    gtk_widget_set_size_request (GTK_WIDGET(button), 52.0, 22.0);
    gtk_widget_show (button);
    gtk_widget_show (box1);
    addWidget(label, box);
    uiButton* c = new uiButton(this, zone, GTK_BUTTON(button));
    GdkColor colorOwn;
    GdkColor colorwn;
    gdk_color_parse ("#c4c4c8", &colorwn);
    gtk_widget_modify_bg (button, GTK_STATE_PRELIGHT, &colorwn);
    gdk_color_parse ("#c0c0c7", &colorOwn);
    gtk_widget_modify_bg (button, GTK_STATE_NORMAL, &colorOwn);
    gtk_signal_connect (GTK_OBJECT (button), "pressed", GTK_SIGNAL_FUNC (uiButton::pressed), (gpointer) c);
    gtk_signal_connect (GTK_OBJECT (button), "released", GTK_SIGNAL_FUNC (uiButton::released), (gpointer) c);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (delet_plug), (gpointer) c);
}

void GTKUI::addExitButton(const char* label, float* zone)
{
    *zone = 0.0;
    GtkWidget* 	button = gtk_button_new_with_mnemonic (label);
    addWidget(label, button);
    GdkColor colorOwn;
    GdkColor colorwn;
    gdk_color_parse ("#c1c1c8", &colorwn);
    gtk_widget_modify_bg (button, GTK_STATE_PRELIGHT, &colorwn);
    gdk_color_parse ("#c1c6c8", &colorOwn);
    gtk_widget_modify_bg (button, GTK_STATE_NORMAL, &colorOwn);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (destroy_event), (gpointer) button);
}

// ---------------------------	Toggle Buttons ---------------------------

struct uiToggleButton : public uiItem
{
    GtkToggleButton* fButton;
    uiToggleButton(UI* ui, float* zone, GtkToggleButton* b) : uiItem(ui, zone), fButton(b) {}
    static void toggled (GtkWidget *widget, gpointer data)
    {
        float	v = (GTK_TOGGLE_BUTTON (widget)->active) ? 1.0 : 0.0;
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()
    {
        float 	v = *fZone;
        fCache = v;
        gtk_toggle_button_set_active(fButton, v > 0.0);
    }
};

void GTKUI::addRecToggleButton(const char* label, float* zone)
{
    *zone = 0.0;
    // make the record button red
    GdkColor colorRed;
    GdkColor colorwn;
    GdkColor colorOwn;
    gdk_color_parse ("#c1c6c8", &colorwn);
    gdk_color_parse ("#f48784", &colorRed);
    gdk_color_parse ("#fba6a6", &colorOwn);
    recbutton = gtk_toggle_button_new_with_mnemonic (label);
    addWidget(label, recbutton);
    gtk_widget_modify_bg (recbutton, GTK_STATE_NORMAL, &colorwn);
    gtk_widget_modify_bg (recbutton, GTK_STATE_PRELIGHT, &colorOwn);
    gtk_widget_modify_bg (recbutton, GTK_STATE_ACTIVE, &colorRed);
    //uiToggleButton* c = new uiToggleButton(this, zone, GTK_TOGGLE_BUTTON(button));
    gtk_signal_connect (GTK_OBJECT (recbutton), "toggled", GTK_SIGNAL_FUNC (recordit), (gpointer) recbutton);
}

// ---------------------------	Check Button ---------------------------

struct uiCheckButton : public uiItem
{
    GtkToggleButton* fButton;
    uiCheckButton(UI* ui, float* zone, GtkToggleButton* b) : uiItem(ui, zone), fButton(b) {}
    static void toggled (GtkWidget *widget, gpointer data)
    {
        float	v = (GTK_TOGGLE_BUTTON (widget)->active) ? 1.0 : 0.0;
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()
    {
        float 	v = *fZone;
        fCache = v;
        gtk_toggle_button_set_active(fButton, v > 0.0);
    }
};

void GTKUI::addCheckButton(const char* label, float* zone)
{
    *zone = 0.0;
    GtkWidget* 	button = gtk_check_button_new_with_mnemonic (label);
    addWidget(label, button);
    GdkColor colorOwn;
    gdk_color_parse ("#c0c0c7", &colorOwn);
    gtk_widget_modify_bg (button, GTK_STATE_PRELIGHT, &colorOwn);
    uiCheckButton* c = new uiCheckButton(this, zone, GTK_TOGGLE_BUTTON(button));
    gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC(uiCheckButton::toggled), (gpointer) c);
}

void GTKUI::addRadioButton(GtkWidget* group, const char* label, float* zone)
{
    *zone = 0.0;
    GtkWidget* 	button = gtk_radio_button_new_with_mnemonic (gtk_radio_button_group (GTK_RADIO_BUTTON (group)), label);
    addWidget(label, button);
    GdkColor colorOwn;
    gdk_color_parse ("#c0c0c7", &colorOwn);
    gtk_widget_modify_bg (button, GTK_STATE_PRELIGHT, &colorOwn);
    uiCheckButton* c = new uiCheckButton(this, zone, GTK_TOGGLE_BUTTON(button));
    gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC(uiCheckButton::toggled), (gpointer) c);

}

void GTKUI::addRadioButtonwithGroup( const char* label, float* zone)
{
    *zone = 0.0;
    buttongroup = gtk_radio_button_new_with_mnemonic  (NULL, label);
    addWidget(label, buttongroup);
    GdkColor colorOwn;
    gdk_color_parse ("#c0c0c7", &colorOwn);
    gtk_widget_modify_bg (buttongroup, GTK_STATE_PRELIGHT, &colorOwn);
    uiCheckButton* c = new uiCheckButton(this, zone, GTK_TOGGLE_BUTTON(buttongroup));
    gtk_signal_connect (GTK_OBJECT (buttongroup), "toggled", GTK_SIGNAL_FUNC(uiCheckButton::toggled), (gpointer) c);
}

void GTKUI::show()
{
    assert(fTop == 0);
    gtk_widget_show  (fBox[0]);
    gtk_widget_show  (fWindow);
}

/**
 * Update all user items reflecting zone z
 */

static gboolean callUpdateAllGuis(gpointer)
{
    UI::updateAllGuis();
    return TRUE;
}

void GTKUI::run()
{
    assert(fTop == 0);
    gtk_widget_show  (fBox[0]);
    gtk_widget_show  (fWindow);
    gtk_timeout_add(40, callUpdateAllGuis, 0);
    gtk_main ();
    stop();
}

/******************************************************************************
*******************************************************************************

								interface GUI

*******************************************************************************
*******************************************************************************/

//----------------------------------------------------------------
//  définition du processeur de signal
//----------------------------------------------------------------

class GUI
{
protected:

public:
    GUI() {}
    virtual ~GUI() {}
    virtual void buildUserInterface(UI* interface) = 0;
};


//----------------------------------------------------------------------------
// 	FAUST generated code
//----------------------------------------------------------------------------


class myGUI : public GUI
{
private:
    float 	fbutton1;
    float 	fcheckbox0;
    float 	fcheckbox1;
    float 	fcheckbox2;
    float 	fcheckbox3;
    float 	fcheckbox4;
    float 	fcheckbox5;
    float 	fcheckbox6;
    float 	fcheckbox7;
    float 	fcheckbox8;
    float 	fcheckbox9;
    float 	fcheckbox10;
    float 	fcheckbox11;
    float 	fcheckbox12;
    float 	fcheckbox13;
    float 	fcheckbox14;
    float 	fcheckbox15;
    float 	fcheckbox16;
    float 	fbutton2;
    float 	fbutton3;
    float 	fsocket1;
    float 	fcbox1;
    float 	fcbox2;
    float 	fbutton4;
    float 	fexpand;
public:
    static void metadata(Meta* m)
    {
        m->declare("name", "jack_capture_settings");
        m->declare("version", "1.00");
        m->declare("author", "brummer");
        m->declare("license", "BSD");
        m->declare("copyright", "it's a present to Kjetil S. Matheussen");
    }
    virtual void buildUserInterface(UI* interface)
    {
// here we insert the channels to lounch given by argv[] appname //
        set_appname();
//----------------------- start build interface -----------------------
        interface->openVerticalBox("");
        if (strcmp(sett, "yes") != 0)
        {
            interface->openExpanderBox("_settings", &fexpand);
            interface->openEventBox(1, " ");
            interface->openVerticalBox("");
        }
        else interface->openVerticalBox("settings");
        interface->openHorizontalBox("");
        interface->openVerticalBox("");
        interface->openEventBox(3, " ");
        interface->openVerticalBox("fileformat");
        interface->openEventBox(2, " ");
        interface->openVerticalBox("");
        interface->addRadioButtonwithGroup("_wav", &fcheckbox0);
        interface->addRadioButton(buttongroup, "_ogg", &fcheckbox1);
        interface->addRadioButton(buttongroup, "_mp3", &fcheckbox2);
        interface->addRadioButton(buttongroup, "_flac", &fcheckbox3);
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->openVerticalBox("");
        interface->openEventBox(3, " ");
        interface->openVerticalBox("bitdepth\nwav only");
        interface->openEventBox(2, " ");
        interface->openVerticalBox("");
        interface->addRadioButtonwithGroup("f_loat", &fcheckbox4);
        interface->addRadioButton(buttongroup,"_32", &fcheckbox5);
        interface->addRadioButton(buttongroup, "_24", &fcheckbox6);
        interface->addRadioButton(buttongroup, "_16", &fcheckbox7);
        interface->addRadioButton(buttongroup, "_8", &fcheckbox8);
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->openHorizontalBox("");
        interface->openEventBox(3, " ");
        interface->openVerticalBox("Channels");
        interface->openEventBox(2, " ");
        interface->openVerticalBox("");
        interface->addRadioButtonwithGroup("2 _Channels", &fcheckbox14);
        interface->addRadioButton(buttongroup, "1 C_hannel", &fcheckbox15);
        interface->addRadioButton(buttongroup, "_auto", &fcheckbox16);
        interface->openVerticalBox("");
        interface->openEventBox(4, " ");
        interface->addDrawBox(0);
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->openVerticalBox("");
        interface->openEventBox(3, " ");
        interface->openVerticalBox("capture");
        interface->openEventBox(2, " ");
        interface->openVerticalBox("");
        interface->addRadioButtonwithGroup(output, &fcheckbox9);
        interface->addRadioButton(buttongroup, input, &fcheckbox10);
        interface->openVerticalBox("");
        interface->addRadioButton(buttongroup, "selec_t Channels", &fcheckbox12);
        interface->addComboBox(0, "Channel", &fcbox1);
        interface->addComboBox(1, "Channel", &fcbox2);
        interface->addButton(" r_efresh ", &fbutton4);
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->openVerticalBox("");
        interface->openVerticalBox("");
        interface->openEventBox(3, " ");
        interface->openHorizontalBox("");
        interface->openEventBox(2, " ");
        interface->openHorizontalBox("");
        interface->addCheckButton( "enable console _db-meter", &fcheckbox13);
        interface->addCheckButton("enable meter_bridge", &fbutton1);
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        interface->closeBox();
        if (strcmp(sett, "yes") != 0)
        {
            interface->closeBox();
            interface->closeBox();
        }
        interface->closeBox();
        if (strcmp(sett, "yes") != 0)
        {
            interface->openHorizontalBox("");
            interface->openHorizontalBox("");
            interface->addRecToggleButton("\n        _record        \n", &fbutton3);
            interface->closeBox();
            interface->openHorizontalBox("");
            interface->addExitButton(" e_xit ", &fbutton2);
            interface->closeBox();
            interface->closeBox();
        }
        else interface->addExitButton("O_k", &fbutton2);
        interface->closeBox();
    }
};

myGUI	GUI;

//-------------------------------------------------------------------------
// 									MAIN
//-------------------------------------------------------------------------



int main(int argc, char *argv[] )
{

    {
        OPTARGS_BEGIN("\033[1;34m jack_capture_settings useage\033[0m\n all parameters are optional\n\n[\033[1;31m--only-settings -o\033[0m] [\033[1;31m--name -n\033[0m] [\033[1;31m--filename -f\033[0m] [\033[1;31m--path -p\033[0m] [\033[1;31m--serverpath -s\033[0m]\n\n"
                      //  "\033[1;34mGive a name and a path where you would save the sound file\033[0m\n\n"
                      "[\033[1;31m--filname\033[0m] or [\033[1;31m-f\033[0m]  ->the name and path where to save the sound file, default is ~/sessionX.xxx\n\n"
                      //  "\033[1;34mYou can set the path to jack_capture if you dont have it installed in your $PATH\nThis argument must include jack_capture and could include parameter witch will give direct to jack_capture\033[0m\n\n"
                      "[\033[1;31m--serverpath\033[0m] or [\033[1;31m-s\033[0m]  ->the path to jack_capture include jack_capture. \nUse like this \033[1;31m -s '\033[0m/path/to/jack_capture parameter parameter\033[1;31m' \033[0mto include parameters to jack_capture\n\n"
                      // "\033[1;34mGive a name to launch a spezified programm\033[0m\n\n"
                      "[\033[1;31m--name\033[0m] or [\033[1;31m-n\033[0m]   ->the name from the program you wont lounch the channels.\n\n"
                      //  "\033[1;34mThis settings are most usefull to lounch jack_capture_gui from a other program\033[0m\n\n"
                      "[\033[1;31m--only-settings\033[0m] or [\033[1;31m-o\033[0m]  ->'\033[1;31myes\033[0m' disable the record and exit button, default is no\n\n"
                      "[\033[1;31m--path\033[0m] or [\033[1;31m-p\033[0m]   ->the path where to save the comandline for jack_capture, default is ~/.jack_capture/ja_ca_setrc\n\n"
                      "\n"
                     )
        {

            OPTARG("--only-settins","-o") sett=OPTARG_GETSTRING();
            OPTARG("--name","-n") appname=OPTARG_GETSTRING();
            OPTARG("--filename","-f") fname=OPTARG_GETSTRING();
            OPTARG("--path","-p") path=OPTARG_GETSTRING();
            OPTARG("--serverpath","-s") serverpath=OPTARG_GETSTRING();
        }
        OPTARGS_END;
    }

    char                rcfilename[256];
    const char*	  home;
    interface = new GTKUI (NULL, &argc, &argv);
    GUI.buildUserInterface(interface);
//------ path to save the gui settings ----------------
    home = getenv ("HOME");
    if (home == 0) home = ".";
    snprintf(rcfilename, 256, "%s/.%src", home, "jack_capture/settings");
    interface->recallState(rcfilename);
    interface->readactuellState(fname, path, input, output, serverpath, sett);
    interface->run();
    // save all values on exit.
    interface->saveState(rcfilename);
    readState(path);
    return 0;
}

