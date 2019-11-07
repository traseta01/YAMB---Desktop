#include "glavniprozor.h"
#include "ui_glavniprozor.h"
#include <vector>
#include <random>
#include <map>
#include <QDebug>
#include <QtGlobal>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <set>
#include <QMessageBox>

using std::vector;
using std::map;
using std::string;
using std::to_string;
using std::find;
using std::set;


/* generate evenly distributed random int from interval */
// from: https://www.learncpp.com/cpp-tutorial/59-random-number-generation/
// Generate a random number between min and max (inclusive)
// Assumes std::srand() has already been called
// Assumes max - min <= RAND_MAX
int getRandomNumber(int min, int max)
{
    static constexpr double fraction { 1.0 / (RAND_MAX + 1.0) };  // static used for efficiency, so we only calculate this value once
    // evenly distribute the random number across our range
    return min + static_cast<int>((max - min + 1) * (std::rand() * fraction));
}

/* broj bacanja od ukupno 3 */
int brBacanja = 0;

/* naja indicator */
int ind_naja = 0;

/* vektor za rand vrednosti */
vector<int> Kockice = {0, 0, 0, 0, 0};
/* vektori izabranih kockica */
vector<int> Izabrane;

/* vektor vrednosti u kolonama  */
vector<int> KolonaDole = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
vector<int> KolonaSlob = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
vector<int> KolonaGore = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
vector<int> KolonaNaja = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


/* map dices rolled to dices selected */
map<QPushButton*, QPushButton*> mapa;
map<QPushButton*, QPushButton*> mapaa;

/* pozicija kockice, kako ne bi morali da biramo sve prilikom upisa u polje */
map<QPushButton*, int> poz_kockica;

/* map column fields  to int for easier reference */
map<QPushButton*, int> mapa_dole;
map<QPushButton*, int> mapa_slob;
map<QPushButton*, int> mapa_gore;
map<QPushButton*, int> mapa_naja;
map<int, bool> mapa_ind_naja;

/* utility functions (to be moved to a separate file)*/
int izdvoj_brojeve(int broj, vector<int>& vk)
{
	int suma = 0;
	for(auto itr = vk.begin(); itr != vk.end(); ++itr)
	{
		if (*itr == broj)
			suma += broj;
	}
	
	return suma;
}

int da_li_je_ful(vector<int>& vek)
{
	if (vek.size() != 5)
		return 0;
	
	/* make a set from vector */
	set<int> skup = set<int>(vek.begin(), vek.end());
	map<int, int> pomap;
	for (auto &i : skup)
	{
		pomap[i] = 0;
	}

	/* candidate for full house */
	if (pomap.size() == 2)
	{
		for (auto &i : vek)
		{
			pomap[i]++;
		}
		
		auto itr = pomap.begin();	
		
		if (itr->second == 2)
		{
			int pomocna = 40;
			pomocna += itr->first*2;
			std::advance(itr, 1);
			pomocna += itr->first*3;
			
			return pomocna;
		}
		else if (itr->second == 3)
		{
			
			int pomocna = 40;
			pomocna += itr->first*3;
			std::advance(itr, 1);
			pomocna += itr->first*2;
			
			return pomocna;
		}
	}

	return 0;
}

int da_li_je_poker(vector<int>& vek)
{
	
	if (vek.size() < 4)
		return 0;
	else if (vek.size() == 4)
	{
		qDebug("Upokeru lin 100 inesto");
		if(vek[0] == vek[1] && vek[0] == vek[2] && vek[0] == vek[3])
		{
			//qDebug("Upokeru lin 100 inesto UIFU");
			return 50 + vek[0]*4;
		}
	}
	else if (vek.size() == 5)
	{
		if(vek[0] == vek[1] && vek[0] == vek[2] && vek[0] == vek[3] && vek[0] == vek[4])
		{
			//qDebug("Upokeru lin 100 inesto UIFU");
			return 50 + vek[0]*4;
		}
	
		/* make a set from vector */
		set<int> skup = set<int>(vek.begin(), vek.end());
		map<int, int> pomap;
		for (auto &i : skup)
		{
			pomap[i] = 0;
		}
    	
		/* candidate for poker */
		if (pomap.size() == 2)
		{
			for (auto &i : vek)
			{
				pomap[i]++;
			}
			
			auto itr = pomap.begin();	
			
			if (itr->second == 1)
			{
				std::advance(itr, 1);
				return 50 + itr->first*4;
			}
			else 
			{
				return 50 + itr->first*4;
			}
		}
	}

	return 0;
}

int da_li_je_kenta(vector<int>& vek)
{
	if(vek.size() != 5)
		return 0;
	
	set<int> testSkup = set<int>(vek.begin(), vek.end());
	
	/* candidate for a straight */
	if(testSkup.size() == 5)
	{
		auto itr1 = testSkup.begin();
		auto itr2 = testSkup.rbegin();
		
		if ( (*itr2 - *itr1) == 4)
		{
			if(*itr1 == 1)
				return 75;
			else
				return 80;
		}
	}
	
	return 0;
}

int da_li_je_yamb(vector<int>& vek)
{
	if(vek.size() != 5)
		return 0;
	else if (vek[0] == vek[1] && vek[0] == vek[2] && vek[0] == vek[3] && vek[0] == vek[4])
		return 80 + vek[0]*5;
	else
		return 0;
}

/* function to generate pesudo random values from 1-6, and put them in a vector */
/* will be redone in the future */
void baci_kockice(vector<int>& kc)
{
	for(int& i : kc)
	{
		i = getRandomNumber(1, 6);
	}
}
/* end utility functions section */

/* CONSTRUCTOR */
GlavniProzor::GlavniProzor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GlavniProzor)
{
    ui->setupUi(this);
	
	/* KOCKICE BACENE I KOCKICE IZABRANE */
	/* connect kockice bacene i kockice izabrane */
	connect(ui->DugmeKockica1, SIGNAL(released()), this, SLOT(IzaberiKockicu()));
	connect(ui->DugmeKockica2, SIGNAL(released()), this, SLOT(IzaberiKockicu()));
	connect(ui->DugmeKockica3, SIGNAL(released()), this, SLOT(IzaberiKockicu()));
	connect(ui->DugmeKockica4, SIGNAL(released()), this, SLOT(IzaberiKockicu()));
	connect(ui->DugmeKockica5, SIGNAL(released()), this, SLOT(IzaberiKockicu()));
	
	connect(ui->DugmeKockicaIzbor1, SIGNAL(released()),this, SLOT(VratiKockicu()));
	connect(ui->DugmeKockicaIzbor2, SIGNAL(released()),this, SLOT(VratiKockicu()));
	connect(ui->DugmeKockicaIzbor3, SIGNAL(released()),this, SLOT(VratiKockicu()));
	connect(ui->DugmeKockicaIzbor4, SIGNAL(released()),this, SLOT(VratiKockicu()));
	connect(ui->DugmeKockicaIzbor5, SIGNAL(released()),this, SLOT(VratiKockicu()));

	/* map kockice bacene to kockice izabrane and vice versa */
	mapa.insert({ui->DugmeKockica1, ui->DugmeKockicaIzbor1});
	mapa.insert({ui->DugmeKockica2, ui->DugmeKockicaIzbor2});
	mapa.insert({ui->DugmeKockica3, ui->DugmeKockicaIzbor3});
	mapa.insert({ui->DugmeKockica4, ui->DugmeKockicaIzbor4});
	mapa.insert({ui->DugmeKockica5, ui->DugmeKockicaIzbor5});
	
	mapaa.insert({ui->DugmeKockicaIzbor1, ui->DugmeKockica1});
	mapaa.insert({ui->DugmeKockicaIzbor2, ui->DugmeKockica2});
	mapaa.insert({ui->DugmeKockicaIzbor3, ui->DugmeKockica3});
	mapaa.insert({ui->DugmeKockicaIzbor4, ui->DugmeKockica4});
	mapaa.insert({ui->DugmeKockicaIzbor5, ui->DugmeKockica5});

	/* map kockice bacene i izabrane to int position */
	poz_kockica.insert({ui->DugmeKockica1, 1});
	poz_kockica.insert({ui->DugmeKockica2, 2});
	poz_kockica.insert({ui->DugmeKockica3, 3});
	poz_kockica.insert({ui->DugmeKockica4, 4});
	poz_kockica.insert({ui->DugmeKockica5, 5});
	
	poz_kockica.insert({ui->DugmeKockicaIzbor1, 1});
	poz_kockica.insert({ui->DugmeKockicaIzbor2, 2});
	poz_kockica.insert({ui->DugmeKockicaIzbor3, 3});
	poz_kockica.insert({ui->DugmeKockicaIzbor4, 4});
	poz_kockica.insert({ui->DugmeKockicaIzbor5, 5});

	
	/* map indikator najave */
	mapa_ind_naja[1] = false;
	mapa_ind_naja[2] = false;
	mapa_ind_naja[3] = false;
	mapa_ind_naja[4] = false;
	mapa_ind_naja[5] = false;
	mapa_ind_naja[6] = false;
	mapa_ind_naja[7] = false;
	mapa_ind_naja[8] = false;
	mapa_ind_naja[9] = false;
	mapa_ind_naja[10] = false;
	mapa_ind_naja[11] = false;
	mapa_ind_naja[12] = false;

	/* KOLONA DOLE */
	/* map kolona dole to values */
	mapa_dole.insert({ui->Dugme1, 1});
	mapa_dole.insert({ui->Dugme2, 2});
	mapa_dole.insert({ui->Dugme3, 3});
	mapa_dole.insert({ui->Dugme4, 4});
	mapa_dole.insert({ui->Dugme5, 5});
	mapa_dole.insert({ui->Dugme6, 6});
	mapa_dole.insert({ui->Dugme7, 7});
	mapa_dole.insert({ui->Dugme8, 8});
	mapa_dole.insert({ui->Dugme9, 9});
	mapa_dole.insert({ui->Dugme10, 10});
	mapa_dole.insert({ui->Dugme11, 11});
	mapa_dole.insert({ui->Dugme12, 12});

	/* connect kolona dole to slot */
	connect(ui->Dugme1, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme2, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme3, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme4, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme5, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme6, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme7, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme8, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme9, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme10, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme11, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));
	connect(ui->Dugme12, SIGNAL(released()), this, SLOT(PoljeZaUpisDoleKliknuto()));

	/* KOLONA SLOB */
	/* map kolona slob to values */
	mapa_slob.insert({ui->DugmeSlob1, 1});
	mapa_slob.insert({ui->DugmeSlob2, 2});
	mapa_slob.insert({ui->DugmeSlob3, 3});
	mapa_slob.insert({ui->DugmeSlob4, 4});
	mapa_slob.insert({ui->DugmeSlob5, 5});
	mapa_slob.insert({ui->DugmeSlob6, 6});
	mapa_slob.insert({ui->DugmeSlob7, 7});
	mapa_slob.insert({ui->DugmeSlob8, 8});
	mapa_slob.insert({ui->DugmeSlob9, 9});
	mapa_slob.insert({ui->DugmeSlob10, 10});
	mapa_slob.insert({ui->DugmeSlob11, 11});
	mapa_slob.insert({ui->DugmeSlob12, 12});
	
	/* connect kolona slob buttons to slot */
	connect(ui->DugmeSlob1, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob2, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob3, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob4, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob5, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob6, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob7, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob8, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob9, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob10, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob11, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));
	connect(ui->DugmeSlob12, SIGNAL(released()), this, SLOT(PoljeZaUpisSlobKliknuto()));


	/* KOLONA GORE */
	/* map kolona gore to values */
	mapa_gore.insert({ui->DugmeGore1, 1});
	mapa_gore.insert({ui->DugmeGore2, 2});
	mapa_gore.insert({ui->DugmeGore3, 3});
	mapa_gore.insert({ui->DugmeGore4, 4});
	mapa_gore.insert({ui->DugmeGore5, 5});
	mapa_gore.insert({ui->DugmeGore6, 6});
	mapa_gore.insert({ui->DugmeGore7, 7});
	mapa_gore.insert({ui->DugmeGore8, 8});
	mapa_gore.insert({ui->DugmeGore9, 9});
	mapa_gore.insert({ui->DugmeGore10, 10});
	mapa_gore.insert({ui->DugmeGore11, 11});
	mapa_gore.insert({ui->DugmeGore12, 12});
	
	/* connect kolona gore buttons to slot */
	connect(ui->DugmeGore1, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore2, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore3, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore4, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore5, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore6, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore7, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore8, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore9, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore10, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore11, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));
	connect(ui->DugmeGore12, SIGNAL(released()), this, SLOT(PoljeZaUpisGoreKliknuto()));


	ui->DugmeKockicaIzbor1->setEnabled(false);
	ui->DugmeKockicaIzbor2->setEnabled(false);
	ui->DugmeKockicaIzbor3->setEnabled(false);
	ui->DugmeKockicaIzbor4->setEnabled(false);
	ui->DugmeKockicaIzbor5->setEnabled(false);

	/* KOLONA NAJA */
	/* map kolona naja to values */
	mapa_naja.insert({ui->DugmeNaja1, 1});
	mapa_naja.insert({ui->DugmeNaja2, 2});
	mapa_naja.insert({ui->DugmeNaja3, 3});
	mapa_naja.insert({ui->DugmeNaja4, 4});
	mapa_naja.insert({ui->DugmeNaja5, 5});
	mapa_naja.insert({ui->DugmeNaja6, 6});
	mapa_naja.insert({ui->DugmeNaja7, 7});
	mapa_naja.insert({ui->DugmeNaja8, 8});
	mapa_naja.insert({ui->DugmeNaja9, 9});
	mapa_naja.insert({ui->DugmeNaja10, 10});
	mapa_naja.insert({ui->DugmeNaja11, 11});
	mapa_naja.insert({ui->DugmeNaja12, 12});
	
	/* connect kolona naja buttons to slot */
	connect(ui->DugmeNaja1, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja2, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja3, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja4, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja5, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja6, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja7, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja8, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja9, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja10, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja11, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));
	connect(ui->DugmeNaja12, SIGNAL(released()), this, SLOT(PoljeZaUpisNajaKliknuto()));


	/* connect other buttons to slots */
	/* button DugmeBaci */
	connect(ui->DugmeBaci, SIGNAL(released()), this, SLOT(DugmeBaciKliknuto()));

	/* button DugmeNovaIgra */
	connect(ui->DugmeNovaIgra, SIGNAL(released()), this, SLOT(NovaIgraKliknuto()));

	/* button izracunaj */
	connect(ui->DugmeIzracunaj, SIGNAL(released()), this, SLOT(IzracunajKliknuto()));
}

/* destructor */
GlavniProzor::~GlavniProzor()
{
    delete ui;
}

void GlavniProzor::DugmeBaciKliknuto()
{
	
	if (brBacanja == 2)
	{
		brBacanja++;
		ui->labelBacanje->setText("BACANJE: " + QString::number(brBacanja));
		baci_kockice(Kockice);
		
		if(ui->DugmeKockica1->isEnabled())
			ui->DugmeKockica1->setText(QString::number(Kockice[0]));
		if(ui->DugmeKockica2->isEnabled())
			ui->DugmeKockica2->setText(QString::number(Kockice[1]));
		if(ui->DugmeKockica3->isEnabled())
			ui->DugmeKockica3->setText(QString::number(Kockice[2]));
		if(ui->DugmeKockica4->isEnabled())
			ui->DugmeKockica4->setText(QString::number(Kockice[3]));
		if(ui->DugmeKockica5->isEnabled())
			ui->DugmeKockica5->setText(QString::number(Kockice[4]));
		ui->DugmeBaci->setEnabled(false);
		return;
	}
	else
	{
		brBacanja++;
		ui->labelBacanje->setText("BACANJE: " + QString::number(brBacanja));
		baci_kockice(Kockice);
		
		if(ui->DugmeKockica1->isEnabled())
			ui->DugmeKockica1->setText(QString::number(Kockice[0]));
		if(ui->DugmeKockica2->isEnabled())
			ui->DugmeKockica2->setText(QString::number(Kockice[1]));
		if(ui->DugmeKockica3->isEnabled())
			ui->DugmeKockica3->setText(QString::number(Kockice[2]));
		if(ui->DugmeKockica4->isEnabled())
			ui->DugmeKockica4->setText(QString::number(Kockice[3]));
		if(ui->DugmeKockica5->isEnabled())
			ui->DugmeKockica5->setText(QString::number(Kockice[4]));
	}
}

void GlavniProzor::IzaberiKockicu()
{
	if(brBacanja == 0)
		return;

	/* get the button that was clicked on using sender() */
	QPushButton *kdugme = (QPushButton*) sender();
	
	/* get the number that was on the button */
	QString vrDugmeta = kdugme->text();
	
	/* convert it to double */
	double vrednost = vrDugmeta.toDouble();
	map<QPushButton*,QPushButton*>::iterator itr = mapa.find(kdugme);
	if(itr != mapa.end())
	{
		//qDebug("OK");
		//QString novi = QString::number(mapa.size());
		//qDebug("%s", qUtf8Printable(novi));

		itr->second->setText(vrDugmeta);
		itr->second->setEnabled(true);
		kdugme->setText("");
		kdugme->setEnabled(false);
		
		
		qDebug("Line 110");
	}
	
	/* store the value in vector */
	Izabrane.push_back(vrednost);
}

void GlavniProzor::VratiKockicu()
{
	
	QPushButton *kdugme = (QPushButton*) sender();
	/* get the number that was on the button */
	QString vrDugmeta = kdugme->text();
	/* convert it to double */
	double vrednost = vrDugmeta.toDouble();
	map<QPushButton*,QPushButton*>::iterator itr = mapaa.find(kdugme);
	if(itr != mapaa.end())
	{
		qDebug("OK189");
		//QString novi = QString::number(mapa.size());
		//qDebug("%s", qUtf8Printable(novi));

		itr->second->setEnabled(true);
		itr->second->setText(vrDugmeta);
		kdugme->setText("");
		kdugme->setEnabled(false);
		
		qDebug("Line 197");
	}
	
	/* delete one occurrence of vrednost from vector Izabrane */
	vector<int>::iterator itrpos = std::find(Izabrane.begin(), Izabrane.end(), vrednost);
	
	if(itrpos != Izabrane.end())
		Izabrane.erase(itrpos);
}

void GlavniProzor::PoljeZaUpisOslobodi()
{
		ui->DugmeKockica1->setText("");
		ui->DugmeKockica2->setText("");
		ui->DugmeKockica3->setText("");
		ui->DugmeKockica4->setText("");
		ui->DugmeKockica5->setText("");
   	
		ui->DugmeKockicaIzbor1->setText("");
		ui->DugmeKockicaIzbor2->setText("");
		ui->DugmeKockicaIzbor3->setText("");
		ui->DugmeKockicaIzbor4->setText("");
		ui->DugmeKockicaIzbor5->setText("");
    	
		ui->DugmeKockica1->setEnabled(true);
		ui->DugmeKockica2->setEnabled(true);
		ui->DugmeKockica3->setEnabled(true);
		ui->DugmeKockica4->setEnabled(true);
		ui->DugmeKockica5->setEnabled(true);

		ui->DugmeKockicaIzbor1->setEnabled(false);
		ui->DugmeKockicaIzbor2->setEnabled(false);
		ui->DugmeKockicaIzbor3->setEnabled(false);
		ui->DugmeKockicaIzbor4->setEnabled(false);
		ui->DugmeKockicaIzbor5->setEnabled(false);

		Izabrane.resize(0);

		ui->DugmeBaci->setEnabled(true);
		ui->labelBacanje->setText("BACANJE: 0");

		brBacanja = 0;
		ind_naja = 0;
}

void GlavniProzor::PoljeZaUpisDoleKliknuto()
{
	/* We want to make an entry to Dole column, so make sure all dices are selected  */
	if(brBacanja == 0)
		return;

	if(ui->DugmeKockica1->isEnabled())
	{
		Izabrane.push_back(Kockice[0]);
	}
	if(ui->DugmeKockica2->isEnabled())
	{
		Izabrane.push_back(Kockice[1]);
	}
	if(ui->DugmeKockica3->isEnabled())
	{
		Izabrane.push_back(Kockice[2]);
	}
	if(ui->DugmeKockica4->isEnabled())
	{
		Izabrane.push_back(Kockice[3]);
	}
	if(ui->DugmeKockica5->isEnabled())
	{
		Izabrane.push_back(Kockice[4]);
	}

	/* add non selected values to Izabrane */
	QPushButton *kdugme = (QPushButton*) sender();
	
	int vrednost = -1;
	int suma = 0;
	

	auto itr = mapa_dole.find(kdugme);
	
	if (itr != mapa_dole.end())
		vrednost = itr->second;
	

	if (vrednost > 1 && KolonaDole[vrednost-2] == -1)
	{
		/* display critcal error message */
		QMessageBox::critical(this, "Greska", "Polje nije na redu!");
		return;
	}
	else if (vrednost < 7)
	{
		suma = izdvoj_brojeve(vrednost, Izabrane);
    	
		kdugme->setText(QString::number(suma));
		
		KolonaDole[vrednost - 1] = suma;
		
		kdugme->setEnabled(false);
    	
		PoljeZaUpisOslobodi();
		
		suma = 0;
		
		for (unsigned i=0; i<6; ++i)
		{
			if(KolonaDole[i] != -1)
				suma += KolonaDole[i];
		}
		
		if (suma > 59)
			suma += 30;
		
		ui->labelRezultat1->setText(QString::number(suma));
	}
	else if (vrednost == 7 || vrednost == 8)
	{
		suma = 0;
		
		if(Izabrane.size() < 5)
		{
			qDebug("MORATE IZABRATI SVE KOCKICE!");
			return;
		}

		for (auto &i : Izabrane)
			suma += i;

		kdugme->setText(QString::number(suma));
		
		KolonaDole[vrednost - 1] = suma;
		
		kdugme->setEnabled(false);
    	
		PoljeZaUpisOslobodi();
		
		if (vrednost == 8)
		{
			suma = (KolonaDole[6] - KolonaDole[7])*KolonaDole[0];
			ui->labelRezultat2->setText(QString::number(suma));
		}
	}
	else if (vrednost == 9)
	{
		suma = 0;
		suma = da_li_je_ful(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaDole[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 10)
	{
		
		suma = da_li_je_poker(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaDole[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 11)
	{
		suma = da_li_je_kenta(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaDole[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 12)
	{
		suma = da_li_je_yamb(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaDole[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	
}

void GlavniProzor::PoljeZaUpisSlobKliknuto()
{
	/* We want to make an entry to Slob column, so make sure all dices are selected  */
	if(brBacanja == 0)
		return;

	if(ui->DugmeKockica1->isEnabled())
	{
		Izabrane.push_back(Kockice[0]);
	}
	if(ui->DugmeKockica2->isEnabled())
	{
		Izabrane.push_back(Kockice[1]);
	}
	if(ui->DugmeKockica3->isEnabled())
	{
		Izabrane.push_back(Kockice[2]);
	}
	if(ui->DugmeKockica4->isEnabled())
	{
		Izabrane.push_back(Kockice[3]);
	}
	if(ui->DugmeKockica5->isEnabled())
	{
		Izabrane.push_back(Kockice[4]);
	}

	/*  find the button that was pressed */
	QPushButton *kdugme = (QPushButton*) sender();

	int vrednost = -1;
	int suma = 0;
	

	auto itr = mapa_slob.find(kdugme);
	
	if (itr != mapa_slob.end())
		vrednost = itr->second;
	
	if (vrednost >= 1 && vrednost < 7)
	{
		suma = izdvoj_brojeve(vrednost, Izabrane);
    	
		kdugme->setText(QString::number(suma));
		
		KolonaSlob[vrednost - 1] = suma;
		
		kdugme->setEnabled(false);
    	
		PoljeZaUpisOslobodi();
		
		suma = 0;
		
		for (unsigned i=0; i<6; ++i)
		{
			if(KolonaSlob[i] != -1)
				suma += KolonaSlob[i];
		}
		
		if (suma > 59)
			suma += 30;
		
		ui->labelRezultat3->setText(QString::number(suma));

		if ((KolonaSlob[6] != -1) && (KolonaSlob[7] != -1) && KolonaSlob[0] != -1)
		{
			suma = (KolonaSlob[6] - KolonaSlob[7])*KolonaSlob[0];
			if (suma < 0)
				suma = 0;

			ui->labelRezultat4->setText(QString::number(suma));
		}
	}
	else if (vrednost == 7 || vrednost == 8)
	{
		//qDebug("tusi 417");
		
		suma = 0;
		
		if(Izabrane.size() < 5)
		{
			qDebug("MORATE IZABRATI SVE KOCKICE!");
			return;
		}

		for (auto &i : Izabrane)
			suma += i;

		kdugme->setText(QString::number(suma));
		
		KolonaSlob[vrednost - 1] = suma;
		
		kdugme->setEnabled(false);
    	
		PoljeZaUpisOslobodi();
		
		if ((KolonaSlob[6] != -1) && (KolonaSlob[7] != -1) && KolonaSlob[0] != -1)
		{
			suma = (KolonaSlob[6] - KolonaSlob[7])*KolonaSlob[0];
			if (suma < 0)
				suma = 0;

			ui->labelRezultat4->setText(QString::number(suma));
		}
	}
	else if (vrednost == 9)
	{
		suma = da_li_je_ful(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaSlob[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 10)
	{
		suma = da_li_je_poker(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaSlob[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 11)
	{
		suma = da_li_je_kenta(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaSlob[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 12)
	{
		suma = da_li_je_yamb(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaSlob[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}

}

void GlavniProzor::PoljeZaUpisGoreKliknuto()
{
	
	/* We want to make an entry to Gore column, so make sure all dices are selected  */

	if(brBacanja == 0)
		return;

	if(ui->DugmeKockica1->isEnabled())
	{
		Izabrane.push_back(Kockice[0]);
	}
	if(ui->DugmeKockica2->isEnabled())
	{
		Izabrane.push_back(Kockice[1]);
	}
	if(ui->DugmeKockica3->isEnabled())
	{
		Izabrane.push_back(Kockice[2]);
	}
	if(ui->DugmeKockica4->isEnabled())
	{
		Izabrane.push_back(Kockice[3]);
	}
	if(ui->DugmeKockica5->isEnabled())
	{
		Izabrane.push_back(Kockice[4]);
	}
	QPushButton *kdugme = (QPushButton*) sender();
	
	int vrednost = -1;
	int suma = 0;
	

	auto itr = mapa_gore.find(kdugme);
	
	if (itr != mapa_gore.end())
		vrednost = itr->second;

	if (vrednost < 12 && KolonaGore[vrednost] == -1)
	{
		/* display critcal error message */
		QMessageBox::critical(this, "Greska", "Polje nije na redu!");
		//qDebug("Polje nije na redu!");
		return;
	}
	else if (vrednost == 12)
	{
		suma = da_li_je_yamb(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaGore[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 11)
	{
		suma = da_li_je_kenta(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaGore[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 10)
	{
		suma = da_li_je_poker(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaGore[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 9)
	{
		suma = da_li_je_ful(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaGore[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if ( vrednost == 8 || vrednost ==7)
	{
		suma = 0;
		
		if(Izabrane.size() < 5)
		{
			qDebug("MORATE IZABRATI SVE KOCKICE!");
			return;
		}

		for (auto &i : Izabrane)
			suma += i;

		kdugme->setText(QString::number(suma));
		
		KolonaGore[vrednost - 1] = suma;
		
		kdugme->setEnabled(false);
    	
		PoljeZaUpisOslobodi();
		
	}
	else if ( vrednost < 7)
	{
		suma = izdvoj_brojeve(vrednost, Izabrane);
    	
		kdugme->setText(QString::number(suma));
		
		KolonaGore[vrednost - 1] = suma;
		
		kdugme->setEnabled(false);
    	
		PoljeZaUpisOslobodi();
		
		suma = 0;
		
		for (unsigned i=0; i<6; ++i)
		{
			if(KolonaGore[i] != -1)
				suma += KolonaGore[i];
		}
		
		if (suma > 59)
			suma += 30;
		
		ui->labelRezultat5->setText(QString::number(suma));

		if (vrednost == 1)
		{
			suma = (KolonaGore[6] - KolonaGore[7])*KolonaGore[0];
			ui->labelRezultat6->setText(QString::number(suma));
		}
	}
}

void GlavniProzor::PoljeZaUpisNajaKliknuto()
{
	if(brBacanja == 0)
		return;

	if(brBacanja > 1 && ind_naja == 0)
	{
		/* display critcal error message */
		QMessageBox::critical(this, "Greska", "Niste najavili!");
		//qDebug("Niste najavili!");
		return;
	}
	else if (brBacanja <= 1 && ind_naja == 0)
	{
		int vrednost = -1;
		qDebug("NAJAVA");

		QPushButton *kdugme = (QPushButton*) sender();
		auto itr = mapa_naja.find(kdugme);
		
		if (itr != mapa_naja.end())
			vrednost = itr->second;
	/* style button if we are in najava */
		QString Buttonstyle = "QPushButton{\
	color:black; \
	background-color: #FFBF00; \
	border: 2px solid gray; \
	padding: 5px; \
} \
QPushButton:pressed{ \
   color:black; \
  	background-color: FFBF00; \
	border: 2px solid gray; \
	padding: 5px; \
}";
		kdugme->setStyleSheet(Buttonstyle);
		ind_naja = vrednost;
		return;
	}
	
	/* We want to make an entry to Naja column, so make sure all dices are selected  */
	if(ui->DugmeKockica1->isEnabled())
	{
		Izabrane.push_back(Kockice[0]);
	}
	if(ui->DugmeKockica2->isEnabled())
	{
		Izabrane.push_back(Kockice[1]);
	}
	if(ui->DugmeKockica3->isEnabled())
	{
		Izabrane.push_back(Kockice[2]);
	}
	if(ui->DugmeKockica4->isEnabled())
	{
		Izabrane.push_back(Kockice[3]);
	}
	if(ui->DugmeKockica5->isEnabled())
	{
		Izabrane.push_back(Kockice[4]);
	}

	/*  find the button that was pressed */
	QPushButton *kdugme = (QPushButton*) sender();

	int vrednost = -1;
	int suma = 0;
	
	/* revert to default style */
	kdugme->setStyleSheet(ui->Dugme1->styleSheet());

	auto itr = mapa_naja.find(kdugme);
	
	if (itr != mapa_naja.end())
		vrednost = itr->second;
	
	if (vrednost != ind_naja)
	{
		/* display critcal error message */
		QMessageBox::critical(this, "Greska", "Niste to najavili!:):P");
		//qDebug("Niste to najavili, EJ ne KRADI!");
		return;
	}
	
	if (vrednost >= 1 && vrednost < 7)
	{
		suma = izdvoj_brojeve(vrednost, Izabrane);
    	
		kdugme->setText(QString::number(suma));
		
		KolonaNaja[vrednost - 1] = suma;
		
		kdugme->setEnabled(false);
    	
		PoljeZaUpisOslobodi();
		
		suma = 0;
		
		for (unsigned i=0; i<6; ++i)
		{
			if(KolonaNaja[i] != -1)
				suma += KolonaNaja[i];
		}
		
		if (suma > 59)
			suma += 30;
		
		ui->labelRezultat7->setText(QString::number(suma));

		if ((KolonaNaja[6] != -1) && (KolonaNaja[7] != -1) && KolonaNaja[0] != -1)
		{
			suma = (KolonaNaja[6] - KolonaNaja[7])*KolonaNaja[0];
			if (suma < 0)
				suma = 0;

			ui->labelRezultat8->setText(QString::number(suma));
		}
	}
	else if (vrednost == 7 || vrednost == 8)
	{
		//qDebug("tusi 417");
		
		suma = 0;
		
		if(Izabrane.size() < 5)
		{
			qDebug("MORATE IZABRATI SVE KOCKICE!");
			return;
		}

		for (auto &i : Izabrane)
			suma += i;

		kdugme->setText(QString::number(suma));
		
		KolonaNaja[vrednost - 1] = suma;
		
		kdugme->setEnabled(false);
    	
		PoljeZaUpisOslobodi();
		
		if ((KolonaNaja[6] != -1) && (KolonaNaja[7] != -1) && KolonaNaja[0] != -1)
		{
			suma = (KolonaNaja[6] - KolonaNaja[7])*KolonaNaja[0];
			if (suma < 0)
				suma = 0;

			ui->labelRezultat8->setText(QString::number(suma));
		}
	}
	else if (vrednost == 9)
	{
		suma = da_li_je_ful(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaNaja[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 10)
	{
		suma = da_li_je_poker(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaNaja[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 11)
	{
		suma = da_li_je_kenta(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaNaja[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
	else if (vrednost == 12)
	{
		suma = da_li_je_yamb(Izabrane);
		kdugme->setText(QString::number(suma));
		KolonaNaja[vrednost - 1] = suma;
		kdugme->setEnabled(false);
		PoljeZaUpisOslobodi();
	}
}

void GlavniProzor::IzracunajKliknuto()
{
	int suma = 0;
	int sumaTotal = 0;

	/* suma kolona */
	QString str = ui->labelRezultat1->text();
	
	if(str.isEmpty())
	{	
		suma += 0;
	}
	else
		suma += str.toInt();
	
	str = ui->labelRezultat3->text();
	
	if(str.isEmpty())
	{	
		suma += 0;
	}
	else
		suma += str.toInt();

	str = ui->labelRezultat5->text();
	
	if(str.isEmpty())
	{	
		suma += 0;
	}
	else
		suma += str.toInt();

	str = ui->labelRezultat7->text();
	
	if(str.isEmpty())
	{	
		suma += 0;
	}
	else
		suma += str.toInt();
	

	sumaTotal += suma;
	ui->labelSumaKolone->setText(QString::number(suma));

	/* suma razlika */
	suma = 0;
	str = ui->labelRezultat2->text();
	
	if(str.isEmpty())
	{	
		suma += 0;
	}
	else
		suma += str.toInt();
	
	str = ui->labelRezultat4->text();
	
	if(str.isEmpty())
	{	
		suma += 0;
	}
	else
		suma += str.toInt();

	str = ui->labelRezultat6->text();
	
	if(str.isEmpty())
	{	
		suma += 0;
	}
	else
		suma += str.toInt();

	str = ui->labelRezultat8->text();
	
	if(str.isEmpty())
	{	
		suma += 0;
	}
	else
		suma += str.toInt();
	
	sumaTotal += suma;
	ui->labelSumaRazlike->setText(QString::number(suma));
	
	/* suma igre dole */
	/* get values of games in columns and write result */
	suma = 0;
	
	for (int i=8; i<12; ++i)
		if(KolonaDole[i] != -1)
			suma += KolonaDole[i];

	sumaTotal += suma;
	ui->labelSumaDole->setText(QString::number(suma));

	/* suma igre slob */
	/* get values of games in columns and write result */
	suma = 0;
	
	for (int i=8; i<12; ++i)
		if(KolonaSlob[i] != -1)
			suma += KolonaSlob[i];

	sumaTotal += suma;
	ui->labelSumaSlob->setText(QString::number(suma));

	/* suma igre gore */
	/* get values of games in columns and write result */
	suma = 0;
	
	for (int i=8; i<12; ++i)
		if(KolonaGore[i] != -1)
			suma += KolonaGore[i];

	sumaTotal += suma;
	ui->labelSumaGore->setText(QString::number(suma));
	
	/* suma igre naja */
	/* get values of games in columns and write result */
	suma = 0;
	
	for (int i=8; i<12; ++i)
		if(KolonaNaja[i] != -1)
			suma += KolonaNaja[i];

	sumaTotal += suma;
	ui->labelSumaNaja->setText(QString::number(suma));
	
	ui->labelKonacan->setText(QString::number(sumaTotal));
	
	/* suma svih igara */
	suma = 0;
	str = ui->labelSumaDole->text();
	suma += str.toInt();
	str = ui->labelSumaGore->text();
	suma += str.toInt();
	str = ui->labelSumaSlob->text();
	suma += str.toInt();
	str = ui->labelSumaNaja->text();
	suma += str.toInt();
	
	ui->labelSumaIgre->setText(QString::number(suma));
}

void GlavniProzor::NovaIgraKliknuto()
{
	ui->DugmeKockica1->setText("");
	ui->DugmeKockica2->setText("");
	ui->DugmeKockica3->setText("");
	ui->DugmeKockica4->setText("");
	ui->DugmeKockica5->setText("");

	ui->DugmeKockica1->setEnabled(true);
	ui->DugmeKockica2->setEnabled(true);
	ui->DugmeKockica3->setEnabled(true);
	ui->DugmeKockica4->setEnabled(true);
	ui->DugmeKockica5->setEnabled(true);

	ui->DugmeKockicaIzbor1->setText("");
	ui->DugmeKockicaIzbor2->setText("");
	ui->DugmeKockicaIzbor3->setText("");
	ui->DugmeKockicaIzbor4->setText("");
	ui->DugmeKockicaIzbor5->setText("");

	ui->DugmeKockicaIzbor1->setEnabled(false);
	ui->DugmeKockicaIzbor2->setEnabled(false);
	ui->DugmeKockicaIzbor3->setEnabled(false);
	ui->DugmeKockicaIzbor4->setEnabled(false);
	ui->DugmeKockicaIzbor5->setEnabled(false);

	
	ui->Dugme1->setEnabled(true);
	ui->Dugme2->setEnabled(true);
	ui->Dugme3->setEnabled(true);
	ui->Dugme4->setEnabled(true);
	ui->Dugme5->setEnabled(true);
	ui->Dugme6->setEnabled(true);
	ui->Dugme7->setEnabled(true);
	ui->Dugme8->setEnabled(true);
	ui->Dugme9->setEnabled(true);
	ui->Dugme10->setEnabled(true);
	ui->Dugme11->setEnabled(true);
	ui->Dugme12->setEnabled(true);

	ui->Dugme1->setText("");
	ui->Dugme2->setText("");
	ui->Dugme3->setText("");
	ui->Dugme4->setText("");
	ui->Dugme5->setText("");
	ui->Dugme6->setText("");
	ui->Dugme7->setText("");
	ui->Dugme8->setText("");
	ui->Dugme9->setText("");
	ui->Dugme10->setText("");
	ui->Dugme11->setText("");
	ui->Dugme12->setText("");


	ui->DugmeSlob1->setEnabled(true);
	ui->DugmeSlob2->setEnabled(true);
	ui->DugmeSlob3->setEnabled(true);
	ui->DugmeSlob4->setEnabled(true);
	ui->DugmeSlob5->setEnabled(true);
	ui->DugmeSlob6->setEnabled(true);
	ui->DugmeSlob7->setEnabled(true);
	ui->DugmeSlob8->setEnabled(true);
	ui->DugmeSlob9->setEnabled(true);
	ui->DugmeSlob10->setEnabled(true);
	ui->DugmeSlob11->setEnabled(true);
	ui->DugmeSlob12->setEnabled(true);

	ui->DugmeSlob1->setText("");
	ui->DugmeSlob2->setText("");
	ui->DugmeSlob3->setText("");
	ui->DugmeSlob4->setText("");
	ui->DugmeSlob5->setText("");
	ui->DugmeSlob6->setText("");
	ui->DugmeSlob7->setText("");
	ui->DugmeSlob8->setText("");
	ui->DugmeSlob9->setText("");
	ui->DugmeSlob10->setText("");
	ui->DugmeSlob11->setText("");
	ui->DugmeSlob12->setText("");

	ui->DugmeGore1->setEnabled(true);
	ui->DugmeGore2->setEnabled(true);
	ui->DugmeGore3->setEnabled(true);
	ui->DugmeGore4->setEnabled(true);
	ui->DugmeGore5->setEnabled(true);
	ui->DugmeGore6->setEnabled(true);
	ui->DugmeGore7->setEnabled(true);
	ui->DugmeGore8->setEnabled(true);
	ui->DugmeGore9->setEnabled(true);
	ui->DugmeGore10->setEnabled(true);
	ui->DugmeGore11->setEnabled(true);
	ui->DugmeGore12->setEnabled(true);

	ui->DugmeGore1->setText("");
	ui->DugmeGore2->setText("");
	ui->DugmeGore3->setText("");
	ui->DugmeGore4->setText("");
	ui->DugmeGore5->setText("");
	ui->DugmeGore6->setText("");
	ui->DugmeGore7->setText("");
	ui->DugmeGore8->setText("");
	ui->DugmeGore9->setText("");
	ui->DugmeGore10->setText("");
	ui->DugmeGore11->setText("");
	ui->DugmeGore12->setText("");

	ui->DugmeNaja1->setEnabled(true);
	ui->DugmeNaja2->setEnabled(true);
	ui->DugmeNaja3->setEnabled(true);
	ui->DugmeNaja4->setEnabled(true);
	ui->DugmeNaja5->setEnabled(true);
	ui->DugmeNaja6->setEnabled(true);
	ui->DugmeNaja7->setEnabled(true);
	ui->DugmeNaja8->setEnabled(true);
	ui->DugmeNaja9->setEnabled(true);
	ui->DugmeNaja10->setEnabled(true);
	ui->DugmeNaja11->setEnabled(true);
	ui->DugmeNaja12->setEnabled(true);

	ui->DugmeNaja1->setText("");
	ui->DugmeNaja2->setText("");
	ui->DugmeNaja3->setText("");
	ui->DugmeNaja4->setText("");
	ui->DugmeNaja5->setText("");
	ui->DugmeNaja6->setText("");
	ui->DugmeNaja7->setText("");
	ui->DugmeNaja8->setText("");
	ui->DugmeNaja9->setText("");
	ui->DugmeNaja10->setText("");
	ui->DugmeNaja11->setText("");
	ui->DugmeNaja12->setText("");

	ui->labelRezultat1->setText("");
	ui->labelRezultat2->setText("");
	ui->labelRezultat3->setText("");
	ui->labelRezultat4->setText("");
	ui->labelRezultat5->setText("");
	ui->labelRezultat6->setText("");
	ui->labelRezultat7->setText("");
	ui->labelRezultat8->setText("");
	
	ui->labelSumaKolone->setText("");
	ui->labelSumaRazlike->setText("");
	ui->labelSumaDole->setText("");
	ui->labelSumaGore->setText("");
	ui->labelSumaSlob->setText("");
	ui->labelSumaNaja->setText("");
	ui->labelSumaIgre->setText("");
	ui->labelKonacan->setText("");
	
	ui->DugmeBaci->setEnabled(true);

	ui->labelBacanje->setText("BACANJE: 0");

	brBacanja = 0;
	ind_naja =0;

	KolonaDole = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	KolonaSlob = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	KolonaGore = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	KolonaNaja = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

	Izabrane.resize(0);
}
