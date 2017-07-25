/* Neuron_Separator_plugin.cpp
 * This is a test plugin, you can use it as a demo.
 * 2017-7-12 : by MK
 */
 
#include "v3d_message.h"
#include <vector>
#include "neuron_separator_plugin.h"
#include <QFileInfo>
#include <iostream>
#include <ctime>
#include <fstream>
#include <iomanip>

using namespace std;
Q_EXPORT_PLUGIN2(Neuron_Separator, neuronSeparator);
 
QStringList neuronSeparator::menulist() const
{
	return QStringList() 
		<<tr("menu1")
		<<tr("menu2")
		<<tr("about");
}

QStringList neuronSeparator::funclist() const
{
	return QStringList()
		<<tr("func1")
		<<tr("func2")
		<<tr("help");
}

void neuronSeparator::domenu(const QString &menu_name, V3DPluginCallback2 &callback, QWidget *parent)
{
	if (menu_name == tr("menu1"))
	{
		v3d_msg("To be implemented.");
	}
	else if (menu_name == tr("menu2"))
	{
		v3d_msg("To be implemented.");
	}
	else
	{
		v3d_msg(tr("This is a test plugin, you can use it as a demo.. "
			"Developed by MK, 2017-7-12"));
	}
}

bool neuronSeparator::dofunc(const QString & func_name, const V3DPluginArgList & input, V3DPluginArgList & output, V3DPluginCallback2 & callback,  QWidget * parent)
{
	if (func_name == tr("separate_neuron"))
	{
		vector<char*> infiles, inparas, outfiles;
		if(input.size() >= 1) infiles = *((vector<char*> *)input.at(0).p);
		if(input.size() >= 2) inparas = *((vector<char*> *)input.at(1).p);
		if(output.size() >= 1) outfiles = *((vector<char*> *)output.at(0).p);
		
		QString swc_circle = inparas.at(0);
		QString inputSWCfile = infiles.at(0);
		QString inputSomas = infiles.at(1);
		qDebug() << inputSWCfile << " " << inputSomas;
		if (swc_circle == "y") this->circle = true;
		else if (swc_circle == "n")	this->circle = false;
		
		this->inputSWCTree = readSWC_file(inputSWCfile);
		QList<NeuronSWC> nodeList = inputSWCTree.listNeuron;
		QList<ImageMarker> somas = readMarker_file(inputSomas);
		this->childsTable = childIndexTable(inputSWCTree);
		vector<long int> somaIDs;
		for (size_t i=0; i<nodeList.size(); ++i)
		{
			this->locLabel[inputSWCTree.hashNeuron.value(nodeList.at(i).n)] = false;
		}
		//cout << locLabel.size() << endl;
		for (QList<ImageMarker>::iterator it=somas.begin(); it!=somas.end(); ++it)
		{
			for (QList<NeuronSWC>::iterator nodeIt=nodeList.begin(); nodeIt!=nodeList.end(); ++nodeIt)
			{
				if (nodeIt->x==it->x-1 && nodeIt->y==it->y-1 && nodeIt->z==it->z-1)
				{
					somaIDs.push_back(nodeIt->n);
					break;
				}
			}
		}
		
		long int wishedID, excludedID;
		for (size_t i=0; i<somaIDs.size()-1; ++i)
		{
			wishedID = somaIDs[i];
			for (size_t j=i+1; j<somaIDs.size(); ++j)
			{
				excludedID = somaIDs[j];
				findPath(childsTable, inputSWCTree, wishedID, excludedID, paths);
				NeuronTree newTree1;
				newTree1.listNeuron = paths.at(0);
				//QString filename = QString::number(wishedID) + "_" + QString::number(excludedID) + ".swc";
				//writeSWC_file(filename, newTree1);

				NeuronSWC wishedSoma = nodeList.at(inputSWCTree.hashNeuron.value(wishedID));
				NeuronSWC excludedSoma = nodeList.at(inputSWCTree.hashNeuron.value(excludedID));
				breakPathMorph(this->inputSWCTree, paths[0], this->childsTable, wishedSoma, excludedSoma);
				//paths.clear();
			}
		}
		somaPathTree(this->somaPath, this->locLabel, paths, inputSWCTree);
		NeuronTree pathTree;
		pathTree.listNeuron = this->somaPath;
		QString pathTreeFileName = "somasTree.swc";
		writeSWC_file(pathTreeFileName, pathTree);
		buildSomaTree();

		breakSomaPathMorph();

		NeuronTree newTree;
		newTree.listNeuron = this->extractedNeuron;
		QString extractedFileName = "soma" + QString::number(wishedID) + ".swc";
		writeSWC_file(extractedFileName, newTree);
	}
	else if (func_name == tr("func2"))
	{
		v3d_msg("To be implemented.");
	}
	else if (func_name == tr("help"))
	{
		v3d_msg("To be implemented.");
	}
	else return false;

	return true;
}

