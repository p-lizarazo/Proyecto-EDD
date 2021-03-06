#ifndef __MALLAS__HXX__
#define __MALLAS__HXX__

#include <vector>
#include <map>
#include <sstream>
#include <math.h>
#include <iostream>
#include <queue>
#include <stack>
#include <limits>

float punto::distancia(punto& v)
{
	float xx, yy, zz, dist;
	xx = x-v.x;
	xx *= xx;
	yy = y-v.y;
	yy *= yy;
	zz = z-v.z;
	zz *= zz;
	dist = sqrt(xx+yy+zz);
	return dist;
}

Objeto::Objeto(string nom, int cant)
{
    nombre=nom;
    grafo.resize(cant);
}

void Objeto::agregarVertice(float xx,float yy,float zz)
{
    punto aux;
    aux.x=xx;
    aux.y=yy;
    aux.z=zz;
    vertices.push_back(aux);
}

void Objeto::agregarCara(vector<int>& relaciones)
{
    caras.push_back(relaciones);
}

int Objeto::cantAristas()
{
    return aristas.size();
}

void Objeto::definirCentro(punto& pnt)
{
    centro = this->vCercano(pnt);
}

void Objeto::agregarArista(int& x, int& y)
{
    pair<int, int> aux;
    if(x<y)
    {
        aux.first = x;
        aux.second = y;
        if(aristas.find(aux)==aristas.end())
        {
            float dist;
            dist = vertices[x].distancia(vertices[y]);
            grafo[x].push_back(pair<float, int>(dist, y));
            grafo[y].push_back(pair<float, int>(dist, x));
            aristas.insert(aux);
        }

        return;
    }
    else if(y<x)
    {
        aux.second = x;
        aux.first = y;
        if(aristas.find(aux)==aristas.end())
        {
            float dist;
            dist = vertices[x].distancia(vertices[y]);
            grafo[x].push_back(pair<float, int>(dist, y));
            grafo[y].push_back(pair<float, int>(dist, x));
            aristas.insert(aux);
        }
        return;
    }
    aux.first = x;
    aux.second = y;
    if(aristas.find(aux)==aristas.end())
    {
        float dist;
        dist = vertices[x].distancia(vertices[y]);
        grafo[x].push_back(pair<float, int>(dist, y));
        grafo[y].push_back(pair<float, int>(dist, x));
        aristas.insert(aux);
    }
}

void Objeto::definirAristas()
{
    aristas.empty();
    int tam = caras.size(), tam2;
    for(int i=0 ; i<tam ; ++i)
    {
        tam2 = caras[i].size();
        for(int j=0 ; j<tam2 ; ++j)
            agregarArista(caras[i][j], caras[i][(j+1)%tam2]);
    }
}


void Objeto::obtVertices(vector<float>& puntos)
{
    puntos[0]=puntos[3]=vertices[0].x;//Asumimos que no est� vacio el vector de vertices
    puntos[1]=puntos[4]=vertices[0].y;
    puntos[2]=puntos[5]=vertices[0].z;
    int tam = vertices.size();
    for(int i=1 ; i<tam ; ++i)
    {
        punto n = vertices[i];
        if(puntos[0]<n.x)
            puntos[0]=n.x;
        if(puntos[1]<n.y)
            puntos[1]=n.y;
        if(puntos[2]<n.z)
            puntos[2]=n.z;
        if(puntos[3]>n.x)
            puntos[3]=n.x;
        if(puntos[4]>n.y)
            puntos[4]=n.y;
        if(puntos[5]>n.z)
            puntos[5]=n.z;
    }
}

pair<vector<int>*, float> Objeto::ruta(int v, int final)
{
    vector<float> dist (vertices.size(), numeric_limits<float>::max());
    priority_queue<pair<float, int>, vector<pair<float, int> >, greater<pair<float, int> > > pq;
    vector<int> *padres = new vector<int> (dist.size(), -1);
    pq.push(pair<float, int>(0, v));
    dist[v] = 0;
    while (!pq.empty()) {
        pair<float, int> top = pq.top();
        pq.pop();
        float d = top.first;
        int u = top.second;
        if (d == dist[u])
        {
            int tam = grafo[u].size();
            for(int i=0 ; i<tam ; ++i)
            {
                int destino = grafo[u][i].second;
                float distDestino = grafo[u][i].first;
                if(dist[u]+distDestino < dist[destino])
                {
                    dist[destino] = dist[u] + distDestino;
                    (*padres)[destino] = u;
                    pq.push(pair<float, int>(dist[destino], destino));
                }
            }
        }
    }
    stack<int> orden;
    int temporal = final;
    while((*padres)[temporal] != -1)
    {
        orden.push(temporal);
        temporal = (*padres)[temporal];
    }
    orden.push(temporal);
    padres->clear();
    while(!orden.empty())
    {
        padres->push_back(orden.top());
        orden.pop();
    }
    return pair<vector<int>*, float>(padres, dist[final]);
}

pair<float, int> Objeto::vCercano(punto& v)
{
    pair<float, int> par;
    float dist;
    punto temp;
    par.first = vertices[0].distancia(v);
    par.second = 0;
    int tam = vertices.size();
    for(int i=1 ; i<tam ; ++i)
    {
    	dist = vertices[i].distancia(v);
    	if(dist<par.first)
    	{
    		par.first = dist;
    		par.second = i;
    	}
    }
    return par;
}

void Objeto::cambiarNombre(string s)
{
	nombre = s;
}

vector<punto>& Objeto::getVert()
{
    return vertices;
}
vector< vector<int> >& Objeto::getCar()
{
    return caras;
}

set<pair<int, int> >& Objeto::getAristas()
{
    return aristas;
}

string& Objeto::getNombre()
{
    return nombre;
}

pair<float, int>& Objeto::getCentro()
{
    return centro;
}

void Malla::agregarObjeto(Objeto* aux)
{
    objetos[aux->getNombre()] = aux;
}

bool Malla::hayObjeto(string ss)
{
    if(buscarObjeto(ss)==objetos.end())
        return false;
    return true;
}
map<string,Objeto*>::iterator Malla::buscarObjeto(string ss)
{
    return objetos.find(ss);
}


Objeto& Objeto::envolvente()
{
    vector<float> fin (6);
    obtVertices(fin);
    Objeto *env = new Objeto ("Envolvente_"+nombre, 8);
    //Agregamos los 8 vertices al objeto combinando coordenadas
    (*env).agregarVertice(fin[0], fin[1], fin[2]);  //x y z
    (*env).agregarVertice(fin[0], fin[1], fin[5]);  //x y -z
    (*env).agregarVertice(fin[0], fin[4], fin[2]);  //x -y z
    (*env).agregarVertice(fin[0], fin[4], fin[5]);  //x -y -z
    (*env).agregarVertice(fin[3], fin[1], fin[2]);  //-x y z
    (*env).agregarVertice(fin[3], fin[1], fin[5]);  //-x y -z
    (*env).agregarVertice(fin[3], fin[4], fin[2]);  //-x -y z
    (*env).agregarVertice(fin[3], fin[4], fin[5]);  //-x -y -z
    //Agregamos las 12 caras que compondr�n la envolvente
    int quemador[36] = {1,2,3, 0,1,2, 2,5,6, 0,2,5, 0,4,5, 0,1,4, 4,5,7, 5,6,7, 3,6,7, 2,3,6, 1,3,4, 3,4,7};
    for(int i=0 ; i<12 ; ++i)
    {
        vector<int> r;
        r.push_back(quemador[3*i]);
        r.push_back(quemador[3*i+1]);
        r.push_back(quemador[3*i+2]);
        (*env).agregarCara(r);
    }
    double xtotal=0, ytotal=0, ztotal=0;
    xtotal+= 4*(fin[0]+fin[3])/8;
    ytotal+= 4*(fin[1]+fin[4])/8;
    ztotal+= 4*(fin[2]+fin[5])/8;
    punto pnt;
    pnt.x = xtotal;
    pnt.y = ytotal;
    pnt.z = ztotal;
    env->definirAristas();
    env->definirCentro(pnt);
    return *env;
}

Objeto& Malla::envolvente(string s)
{
    return buscarObjeto(s)->second->envolvente();
}

Objeto& Malla::envolvente()
{
    vector<float> fin (6);
    vector<float> temp (6);
    objetos.begin()->second->obtVertices(fin);
    for(map<string, Objeto*>::iterator it = objetos.begin() ; it!=objetos.end() ; ++it)
    {
        if(it->second->getVert().empty())
            continue;
        it->second->obtVertices(temp);
        for(int j=0 ; j<3 ; ++j)
            if(temp[j]>fin[j])
                fin[j]=temp[j];
        for(int j=3 ; j<6 ; ++j)
            if(temp[j]<fin[j])
                fin[j]=temp[j];
    }
    int a = 1;
    stringstream ss;
    ss << a;
    string str = ss.str();
    while(hayObjeto("Envolvente_Malla_"+str))
    {
        a++;
        ss.str("");
        ss << a;
        str = ss.str();
    }
    Objeto *env = new Objeto ("Envolvente_Malla_"+str, 8);
    //Agregamos los 8 vertices al objeto combinando coordenadas
    (*env).agregarVertice(fin[0], fin[1], fin[2]);  //x y z
    (*env).agregarVertice(fin[0], fin[1], fin[5]);  //x y -z
    (*env).agregarVertice(fin[0], fin[4], fin[2]);  //x -y z
    (*env).agregarVertice(fin[0], fin[4], fin[5]);  //x -y -z
    (*env).agregarVertice(fin[3], fin[1], fin[2]);  //-x y z
    (*env).agregarVertice(fin[3], fin[1], fin[5]);  //-x y -z
    (*env).agregarVertice(fin[3], fin[4], fin[2]);  //-x -y z
    (*env).agregarVertice(fin[3], fin[4], fin[5]);  //-x -y -z
    //Agregamos las 12 caras que compondr�n la envolvente
    int quemador[36] = {1,2,3, 0,1,2, 2,5,6, 0,2,5, 0,4,5, 0,1,4, 4,5,7, 5,6,7, 3,6,7, 2,3,6, 1,3,4, 3,4,7};
    for(int i=0 ; i<12 ; ++i)
    {
        vector<int> r;
        r.push_back(quemador[3*i]);
        r.push_back(quemador[3*i+1]);
        r.push_back(quemador[3*i+2]);
        (*env).agregarCara(r);
    }
    double xtotal=0, ytotal=0, ztotal=0;
    xtotal+= 4*(fin[0]+fin[3])/8;
    ytotal+= 4*(fin[1]+fin[4])/8;
    ztotal+= 4*(fin[2]+fin[5])/8;
    punto pnt;
    pnt.x = xtotal;
    pnt.y = ytotal;
    pnt.z = ztotal;
    env->definirAristas();
    env->definirCentro(pnt);
    return *env;
}

pair<pair<float,int>,map<string, Objeto*>::iterator> Malla::vCercano(punto& v)
{
	pair<pair<float, int>, map<string, Objeto*>::iterator > fin;
	pair<float, int> par;
	int tam = objetos.size();
	fin.first = objetos.begin()->second->vCercano(v);
	fin.second = objetos.begin();
	for(map<string, Objeto*>::iterator it=objetos.begin() ; it!=objetos.end() ; ++it)
	{
		par = it->second->vCercano(v);
		if(par.first < fin.first.first)
		{
			fin.first = par;
			fin.second = it;
		}
	}
	return fin;
}

map<string, Objeto*>& Malla::getObjetos()
{
    return objetos;
}

#endif //__MALLAS__HXX__
