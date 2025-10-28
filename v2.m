clc,clearvars,close all
fichier = fileread("Traces.txt");
eval(fichier)


%Ici on a les parametre utilisateurs qui sont ajooutés par labview
function [A,B,C,D]=orderCorners(Corners)%celle la elle marche sur c'est l'exo un de la partie matlab

Distance=[sqrt(Corners(:,1).^2+Corners(:,2).^2),Corners(:,1),Corners(:,2)]';%Le point le plus proche est A

[min_val, indice] = min(Distance(1, :));

A=[Distance(2,indice),Distance(3,indice)];

Distance=sortrows(Distance')';

DistWA=[Distance(1,2:end);Distance(2,2:end);Distance(3,2:end)];%On enlève A pour éviter d'avoir un angle 0 qui fausserait les angles

Angle = [atan2((DistWA(3,:)-A(2)),DistWA(2,:)-A(1));DistWA(2,:);DistWA(3,:)];%ATTENTION techniquement ici il y a une div par zero mais elle pose pas de pb car sortrows la met a la fin


Angle=sortrows(Angle')';%pour simplifier on ordonne les angles du plus petit au plus grand


B=[Angle(2,1),Angle(3,1)];
C=[Angle(2,2),Angle(3,2)];
D=[Angle(2,3),Angle(3,3)];

end

function [X Y] = ApplyM( M, x, y)
homogenes=[x,y,ones(length(x),1)]';%on rajoute la partie homogène aux coordonnées
NewHomogene=M*homogenes;
X1=(NewHomogene(1,:)./NewHomogene(3,:));%On divise X par l'homogénisation
Y1=(NewHomogene(2,:)./NewHomogene(3,:));%pareil pour y
Ordonne=[X1;Y1];
propre=sortrows(Ordonne')';
X=propre(1,:);
Y=propre(2,:);
end

function[M]=ComputeM(A,B,C,D, Width, Height)%ca c'est la fonction partie 3

A_Matrix=[A(1,1) A(1,2) 1 0 0 0 0 0;
    0 0 0 A(1,1) A(1,2) 1 0 0;
    B(1,1) B(1,2) 1 0 0 0 -Width*B(1,1) -Width*B(1,2);
    0 0 0 B(1,1) B(1,2) 1 0 0;
    C(1,1) C(1,2) 1 0 0 0 -Width*C(1,1) -Width*C(1,2);
    0 0 0 C(1,1) C(1,2) 1 -Height*C(1,1) -Height*C(1,2);
    D(1,1) D(1,2) 1 0 0 0 0 0;
    0 0 0 D(1,1) D(1,2) 1 -Height*D(1,1) -Height*D(1,2)];
%On ose nos matrices pour faire résoudre le système a matlab(c'est la
%matrice A donnée dans l'énnoncé

b=[0;0;Width;0;Width;Height;0;Height];

x=A_Matrix\b;

M=[x(1) x(2) x(3); x(4) x(5) x(6); x(7) x(8) 1];%On met la matrice de transformation au bon format (sinon on avait un vecteur)

end

function[ParamStr,yfitted,DC]=FindTrace(X, Y, Width, Height)

[polynome, gof1] = fit(X',Y','poly3');
RMSE1=gof1.rmse;
coeffValues=coeffvalues(polynome);
%On fit un polynome de degré 3


DC=(max(Y)+min(Y))/2;%Trouve la valeur continue 
Y1=Y-(max(Y)+min(Y))/2;
[SinusPur, gof2] = fit(X', Y1', 'sin1');%ici il ne me donne que la partie sans DC
coeffValuesPur=coeffvalues(SinusPur);%je récupere les valeurs 
RMSE2=gof2.rmse;
%On fit un sinus

%On compare les fit et on renvoie le meilleur
if (RMSE1<=RMSE2)
    yfitted=polynome;
    DC=0;
    ParamStr=["a:",num2str(round(coeffValues(1),2)),", b:",num2str(round(coeffValues(2),2)),", c:",num2str(round(coeffValues(3),2)),", d:",num2str(round(coeffValues(4),2)),", RMSE:",num2str(round(RMSE1,2))];
    %On renvoie le bon string parametre et le bon cfit pour etre tracés et
    %légendés dans le pdf
end
if(RMSE2<RMSE1)
    yfitted=SinusPur;
    F=num2str(round(coeffValuesPur(2)/(2*pi),2));
    P=num2str(round(180*coeffValuesPur(3)/pi,2));
    ParamStr=["A:",num2str(round(coeffValuesPur(1),2)),"[V], DC:",num2str(round(DC,2)),"[V], F:",F,"[Hz], P:",P,"[deg], RMSE:",num2str(round(RMSE2,2))];
end

end

Width=max(Corners(:,1));
Height=max(Corners(:,2));
[A,B,C,D]=orderCorners(Corners);
M=ComputeM(A,B,C,D, Width, Height);


% les lignes suivantes permettent de dessiner les points pour toutes les
% traces. 
figure('WindowState','maximized');
hold on;
legends = cell(1, length(Traces));
handles= cell(1, length(Traces));


for i=1:length(Traces)

    x=(Traces{1,i}(:,1));%matrice colonne
    y=(Traces{1,i}(:,2));%matrice colonne
    [X, Y] = ApplyM( M, x, y);%On redresse l'image

    %FindTrace(X,Y,Width,Height);
    X=X/Width;
    Y=(Y/Height)*2-1;%On normalise les axes

    [ParamStr, yfitted, DC]=FindTrace(X, Y, Width, Height);%Trouve les traces
    
    plot(X,Y,style_of_points,'Color',color_of_points{i},'MarkerSize',size);%On trace les points avc le style et la couleur que veut l'utilisateur
    new_function = @(X) (yfitted(X)+DC);%Si on avait un sinus il faut rajouter la partie continue
    handles{i}=plot(sort(X),new_function(sort(X)));%on enregitre la trace dans un array pour la légende
    set(handles{i}, 'Color',color_of_points{i});%Donne a l'array quo gere la légende a culeur qu'on a utilisé
    ParamStr = strjoin(ParamStr, ' ');
    legends{i}=ParamStr;
    yligne=zeros(length(X));
    plot(X, yligne,'-','Color',[0 0 0], 'LineWidth', 0.5);

end% elle on peut pas la supprimer parcequ'il faut appeler pour toutes les traces.


%Ici on gere l'affichage du quadrillage des échelles, du tritre et de a
%légende pour un résultat propre
heure_actuelle = datestr(now, 'HH:MM:SS');

grid on;
box on;
yticks ([0]);
xticklabels({"" "" "" "" "" "" "" "" ""});
axis square;
xlabel('0.1[s]/div');
ylabel('0.2[V]/div');
xlim([0,1]);
ylim([-1,1]);
title(['SRC: ',titre_png,'.png le ',date,' @ ',heure_actuelle]);
XTickLabel=({});




legend([handles{:}],legends,'location','southoutside');
hold off;
%enregistre la figure au format pdf
print(titre_png,'-dpdf','-fillpage');