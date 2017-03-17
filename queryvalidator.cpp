#include "queryvalidator.h"

QueryValidator::QueryValidator()
{

}

QString QueryValidator::removeUnneededContent(QString query)
{
    //!@brief --brisanje jednolinijskog komentara iz stringa
    query = query.remove(QRegExp("#[^\n]*|--[^\n]*"));

    //!@brief --brisanje višelinijskog komentara iz stringa, @attention /*! */ se NE BRIŠE jer to nije komentar, već specific code, obojen je u crveno
    query = query.remove(QRegExp("/\\*([^*]|[\r\n]|(\\*+([^*/]|[\r\n])))*\\*+/"));

    //!@brief --brisanje višelinijskog komentara koji nije zatvoren do kraja, ukoliko ga ima
    if (query.contains("/*"))
        query = query.remove(query.lastIndexOf("/*"), query.length());

    //!@brief --brisanje \t \n i sličnih znakova
    query = query.simplified();

    return query;
}

QStringList QueryValidator::parseToQuerys(QString query, bool explain)
{
    QStringList querys;

    if (!query.contains("create trigger", Qt::CaseInsensitive))//ne sadrži trigger
    {
        //!@brief --razbijanje stringa po defaultnom delimiteru
        //querys = query.split(";", QString::SkipEmptyParts); //skip je preskočen jer ne čistim string od viška ;
        querys = query.split(";");

        if (querys.last().isEmpty())
            querys.removeLast();//poslednji je višak!!!

        //!@brief --brisanje space na početku i na kraju ukoliko postoji, i brisanje praznih upita
        for (int i = querys.count() - 1; i >= 0; i--)
        {
            querys.replace(i, querys.at(i).trimmed());
            querys.replace(i, querys.at(i) + ";"); //dodajem na kraj separator jer je u splitu on izbačen
        }
    }
    else//sadrži trigger i treba da ga izdvojimo
    {
        //!@brief --prvo se string razbije reč po reč
        QStringList recPoRec = query.split(" ");
        QStringList poTriggeru;

        ///explain query plan

        for (int i = 0; i < recPoRec.count(); i++)
        {
            //u ovim slučajevima je trigger
            if (
                    (recPoRec.at(i).toLower() == "explain" && recPoRec.at(i+1).toLower() == "query" && recPoRec.at(i+2).toLower() == "plan" &&  recPoRec.at(i+3).toLower() == "create" && recPoRec.at(i+4).toLower() == "trigger")
                    ||
                    (recPoRec.at(i).toLower() == "explain" && recPoRec.at(i+1).toLower() == "create" && recPoRec.at(i+2).toLower() == "trigger")
                    ||
                    (recPoRec.at(i).toLower() == "create" && recPoRec.at(i+1).toLower() == "trigger")
               )
            {
                QString trigger = "";

                //motaj dok ne dođeš do end; što znači da je kraj triggera
                while(recPoRec.at(i).toLower() != "end;")
                {
                    trigger += recPoRec.at(i);
                    trigger += " ";
                    i++;
                    if (i == recPoRec.count()-1) //ako nema end; onda završi petlju
                        break;
                }

                poTriggeru << trigger + recPoRec.at(i);
            }
            else
            {
                QString upit = "";
                //motaj dok ne dođeš do kraja upita tj. znaka ;
                while (!recPoRec.at(i).contains(";")  && i != recPoRec.count()-1)
                {
                    upit += recPoRec.at(i);
                    upit += " ";
                    i++;
                }

                upit = upit.append(recPoRec.at(i));

                //ukoliko se na kraju upita ne stavi ';' onda ja stavljam, ovo mi olakšava izvšavanje upit
                if (!upit.contains(";"))
                    upit.append(";");

                poTriggeru << upit;
            }

        }
        querys = poTriggeru;
    }

    if (explain == true)//onda treba ispred svakog upita dodati explain query plan
    {
        for (int i = 0; i < querys.count(); i++)
            querys.replace(i, "explain query plan " + querys.at(i));
    }

    for (int i = querys.count() - 1; i >= 0; i--)//čišćenje od viška
    {
        if (querys.at(i) == "" || querys.at(i) == " ")
            querys.removeAt(i);
    }

    return querys;
}
