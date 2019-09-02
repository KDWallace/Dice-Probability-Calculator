#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>

double probability(int,double);
static double igf(double,double);
//double gamma(double);
int main()
{
  //array of the number of rolls of that number. E.G: roll[4] = 20 means that the number 4 was rolled 20 times
  int max = 0;
  bool txt = false, d100 = false;
  std::ifstream file;
  int roll[20] = {0};
  std::string DIR;
  do
  {
    std::cout << "File name: ";
    std::cin >> DIR;
    file.open(DIR);
    if (!file)
    {
      txt = true;
      file.open(DIR + ".txt");
    }
  }
  while(!file);

  std::cout << "Dice in use: d";
  while (1)
  {
    try
    {
      std::cin >> max;
      if (max == 2 || max == 4 || max == 6 || max == 8 || max == 10 || max == 12 || max == 20)
        break;
      else if (max == 100)
      {
        d100 = true;
        max = 10;
        break;
      }
    }
    catch (std::exception e) {
      std::cout << "Invalid input\n";
    }
  }

  std::string tmp;
  int pos, j = 0;
  file.clear();
  while (getline(file,tmp))
  {
    if (tmp == "")
      continue;
    std::stringstream conv(tmp);
    conv >> pos;
    if (d100)
    {
      if (pos % 10 == 0)
        pos = pos / 10;
      else if (j > 1)
        d100 = false;
      else
      {
        d100 = false;
        j = 0;
        for (int i = 0; i < 20; i ++)
        {
          roll[i] = 0;
        }
        file.clear();
        file.seekg(0,std::ios::beg);
        continue;
      }
    }
    if (pos > max || pos < 1)
    {
      std::cerr << "Out of expected range, value of " << pos << " on line " << j+1 <<  std::endl;
      exit(0);
      break;
    }
    roll[pos-1] += 1;
    j++;
  }
  file.close();
  double chisqrd = 0;
  double expected = (1/(double)max)*j;
  for (int i = 0; i < max; i++)
    chisqrd += (((roll[i]-expected)*(roll[i]-expected))/expected);
  std::cout << "\n\nChi-Squared value: " << chisqrd << "\n";
  double P = probability(max-1,chisqrd);
  std::cout << "Probability of random chance: " << P << "%\n";
  std::ofstream report;
  std::string fileName = "Dice stats for " + DIR;
  if (txt)
    fileName += ".txt";
  report.open(fileName);
  report << "\t\t-= " << fileName << " =-\n";
  report << "\nTotal rolls:\t" << j;
  for (int i = 0; i < max; i++)
    report << "\nNumber of " << i+1 << "'s rolled:\t" << roll[i];
  report << "\n\nChi Squared value:\t\t\t" << chisqrd;
  report << "\nProbability of chance (alpha value):\t" << P << "%";
  if (P < 5)
  {
    std::cout << "\nWARNING! Dice are below the null-hypothesis value of 5% and should be considered as biased\nThese are not legal dice!\n";
    report << "\n\nWARNING! Dice are below the null-hypothesis value of 5% and should be considered as biased\nThese are not legal dice!";
  }
  else if (P < 10)
  {
    std::cout << "\nRelatively unbiased and is above the minimum threshold of 5%. However, may be slightly biased as below 10% probability.\n";
    report << "\n\nThese dice aren't necessarily biased and pass the 5% minimum. However, may be slightly biased as below 10% probability.";
  }
  else if (P < 20)
  {
    std::cout << "\nRelatively unbiased and most likely fine\n";
    report << "\n\nThese dice are relatively fine. They pass the hypothesis minimum of 5% and are above the mathematical 10% minimum by a bit. I'm sure they're fine to use";
  }
  else if (P < 50)
  {
    std::cout << "\nThese dice are relatively unbiased and are safe to use in a game, would most likely be casino legal too!\n";
    report << "\n\nThese dice are relatively unbiased and are safe to use in a game, would most likely be casino legal too!";
  }
  else if (P < 75)
  {
    std::cout << "\nHighly unbiased and within the centre of the distribution. Reliable and random.\n";
    report << "\n\nHighly unbiased and within the centre of the distribution. Reliable and random.";
  }
  else
  {
    std::cout << "\nIncredibly unbiased. To get this high of a percentage is very impressive. These dice are very fair.\n";
    report << "\n\nIncredibly unbiased. To get this high of a percentage is very impressive. These dice are as fair as it gets.";
  }

  if (j < max*5)
  {
    std::cout << "\n\nWARNING! The results of this test are not reliable for less than " << 5 * max << " rolls (5*number of sides). There are only " << j << " rolls in this dataset\n";
    report << "\n\nNOTE: This dataset contains less than the recommended amount of rolls for a reliable result. For this dice, the recommended is " << 5 * max << " (5*number of sides)";
  }
  report.close();
  return 0;
}

double probability(int DoF, double chisquared)
{
  //DegFree = degrees of freedom. a d6 will have DoF = 5, a d8 will have 7 etc.
  double K = ((double)DoF) * 0.5;
  double X = chisquared * 0.5;
  double prob = igf(K, X);
  prob /= tgamma(K);
  prob = 1 - prob;
  return prob * 100;
}

static double igf(double S, double Z)
{
    if(Z < 0.0)
    {
	return 0.0;
    }
    double Sc = (1.0 / S);
    Sc *= pow(Z, S);
    Sc *= exp(-Z);
    double Sum = 1.0;
    double Nom = 1.0;
    double Denom = 1.0;
    for(int I = 0; I < 200; I++)
    {
	Nom *= Z;
	S++;
	Denom *= S;
	Sum += (Nom / Denom);
    }
    return Sum * Sc;
}
