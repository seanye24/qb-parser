#include "window.h"
#include <QDir>
#include <QtDebug>

// constructor
Window::Window(QWidget *parent) : QWidget(parent) {
  showNormal();

  // create categories map
  categories = new std::unordered_map<std::string, int>;
  *categories = {{"mythology", 14},
                 {"literature", 15},
                 {"trash", 16},
                 {"science", 17},
                 {"history", 18},
                 {"religion", 19},
                 {"geography", 20},
                 {"fine arts", 21},
                 {"social science", 22},
                 {"philosophy", 25},
                 {"current events", 26},
                 {"myth", 14},
                 {"lit", 15},
                 {"tr", 16},
                 {"sci", 17},
                 {"hist", 18},
                 {"reli", 19},
                 {"geo", 20},
                 {"fa", 21},
                 {"ss", 22},
                 {"phil", 25},
                 {"ce", 26}};

  // create and fill tossups vector
  tossups = new std::unordered_map<std::string, std::vector<tossup_data *> *>;
  fillTossups(tossups);

  // create phrases map
  phrases = new std::unordered_map<std::string, int>;

  // create search button
  search = new QPushButton("Search", this);

  // create quit button
  quit = new QPushButton("Quit", this);

  // create tossup entry
  tossup = new QLineEdit(this);

  // create category entry
  category = new QLineEdit(this);

  // create number of words entry
  num_words = new QLineEdit(this);

  // create tossup label
  tossup_label = new QLabel("Tossup", this);
  tossup_label->setAlignment(Qt::AlignCenter);

  // create category label
  category_label = new QLabel("Category", this);
  category_label->setAlignment(Qt::AlignCenter);

  // create category label
  num_words_label = new QLabel("Number of Words", this);
  num_words_label->setAlignment(Qt::AlignCenter);

  // create search results
  results = new QTextEdit(this);

  // setup grid
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(tossup_label, 0, 0);
  layout->addWidget(category_label, 0, 1);
  layout->addWidget(num_words_label, 0, 2);
  layout->addWidget(tossup, 1, 0);
  layout->addWidget(category, 1, 1);
  layout->addWidget(num_words, 1, 2);
  layout->addWidget(search, 1, 3);
  layout->addWidget(quit, 1, 4);
  layout->addWidget(results, 2, 0, 1, 5);
  this->setLayout(layout);

  // set up signal and slots
  connect(quit, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));
  connect(search, SIGNAL(clicked(bool)), this, SLOT(query()));
  connect(tossup, SIGNAL(returnPressed()), this, SLOT(query()));
  connect(category, SIGNAL(returnPressed()), this, SLOT(query()));
  connect(num_words, SIGNAL(returnPressed()), this, SLOT(query()));
}

// fill tossups map from csv file
// only done when program is first ran
void Window::fillTossups(
    std::unordered_map<std::string, std::vector<tossup_data *> *> *tossups) {

  std::string cluesFile = "/Users/sean/projects/qb-parser/t_a.csv";
  std::ifstream infile(cluesFile);   // open file in input stream
  std::string answer, question, cat; // record tossup, answer, and category
  int category;                      // category data

  // loop through tossups file
  while (std::getline(infile, question, ',')) {

    // get tossup content and category
    std::getline(infile, answer, ',');
    std::getline(infile, cat);
    category = std::stoi(cat);

    // initialize new tossup struct
    tossup_data *tossup = new tossup_data(answer, question, category);

    if (tossups->count(answer) == 0) {
      // add new answer key to map and initialize vector of tossups and add
      // current answer/tossup
      std::vector<tossup_data *> *tossup_list = new std::vector<tossup_data *>;
      tossup_list->push_back(tossup);
      (*tossups)[answer] = tossup_list;
    } else {
      // push answer to existing vector of tossups for the matched key
      (*tossups)[answer]->push_back(tossup);
    }
  }
}

// searches for tossups based on user entered tossup name
void Window::query() {

  // clear window and phrase map from previous run
  results->clear();
  phrases->clear();

  // if empty tossup name or phrase length, inform user of error
  if (tossup->text().size() == 0) {
    results->append("Please enter a tossup.");
    return;
  }

  auto compare = [this](auto p1, auto p2) {
    return phrases->at(p1) < phrases->at(p2);
  };
  std::priority_queue<std::string, std::vector<std::string>, decltype(compare)>
      phrase_order(compare); // stores phrases by order of freq
  std::string tossup_name =
      (tossup->text()).toUtf8().constData(); // get name of tossup entered
  std::string category_name =
      (category->text()).toUtf8().constData(); // get name of category entered
  int phrase_len, counter = 0;                 // phrase len and tossup counter
  bool category_bypass = false; // bypasses category check if category is blank

  // try to stoi num_words entered
  // inform user of error if non-numerical input given
  // if empty, set phrase length to 0
  try {
    phrase_len = std::stoi(num_words->text().toUtf8().constData());
  } catch (const std::invalid_argument &ia) {
    if (num_words->text().size() > 0) {
      results->append("Number of words entered incorrectly.");
      return;
    } else {
      phrase_len = 0;
    }
  }

  if (category_name.size() > 0 && categories->count(category_name) == 0) {
    // check if category doesn't exist
    results->append("Category not found.");
    return;
  } else if (category_name.size() == 0) {
    // turn off category checking
    category_bypass = true;
  }

  // begin query
  // loop through answers of tossups map
  for (std::pair<std::string, std::vector<tossup_data *> *> element :
       *tossups) {
    // if answer key "matches" user entered tossup record all mapped tossups
    if ((int)element.first.find(tossup_name) != -1) {
      std::vector<tossup_data *> *vec =
          element.second; // vector of tossups mapped by answer key

      // loop through vector of tossups
      for (int i = 0; i < (int)vec->size(); i++) {

        // if category specified, check if categories match
        if (category_bypass ||
            (*vec)[i]->category == (*categories)[category_name]) {
          if (phrase_len > 0) {
            // parse tossup if phrase length > 0
            parse((*vec)[i]->question, phrase_len);
          } else if (phrase_len == 0) {
            // print tossup if phrase length = 0
            results->append("Tossup: " +
                            QString::fromStdString((*vec)[i]->question));
            results->append(
                "Answer: " + QString::fromStdString((*vec)[i]->answer) + "\n");
          }
          // record number of tossups hit
          counter++;
        }
      }
    }
  }

  // if printing tossups, when finished, record counter info at top and exit
  if (phrase_len == 0) {
    results->setText(QString::fromStdString(std::to_string(counter)) +
                     " found tossups.\n\n" + results->toPlainText());
    results->moveCursor(QTextCursor::Start); // scroll window to top
    return;
  }

  // push all phrases into priority, ordering by freq
  // ignore phrases of only freq 1
  for (std::pair<std::string, int> element : *phrases) {
    if (element.second < 2)
      continue;
    phrase_order.push(element.first);
  }

  // if printing phrases, print counter info at top
  results->append(QString::fromStdString(std::to_string(counter)) +
                  " found tossups.");
  results->append(QString::fromStdString(std::to_string(phrase_order.size())) +
                  " found phrases.\n");

  // pop all phrases from p queue
  while (!phrase_order.empty()) {
    std::string phrase = phrase_order.top();
    QString result =
        "Freq: " +
        QString::fromStdString((std::to_string((*phrases)[phrase]))) +
        " Phrase: " + QString::fromStdString(phrase);
    results->append(result);
    phrase_order.pop();
  }

  // scroll window to top
  results->moveCursor(QTextCursor::Start);
}

// parses tossup questions into user specified phrase length
void Window::parse(std::string &question, int phrase_len) {
  int first_index = 0,
      last_space_index = 0; // first and last index for substring

  // loop through question until last possible space is reached
  while (true) {

    // calculate where the back index of substring should be based on user
    // entered phrase length
    for (int i = 0; i < phrase_len; i++) {
      last_space_index =
          question.find(" ", last_space_index + 1); // jump spaces
      if (last_space_index == -1)
        return; // return if no spaces left
    }

    // parse tossup with substring
    std::string phrase =
        question.substr(first_index, last_space_index - first_index);

    // add phrase to map
    if (phrases->find(phrase) == phrases->end()) {
      (*phrases)[phrase] = 1; // set freq to 1 if new
    } else {
      (*phrases)[phrase]++; // increment freq if old
    }

    first_index = question.find(" ", first_index) + 1; // find next first index
    last_space_index = first_index; // set up last index based on front
  }
}
