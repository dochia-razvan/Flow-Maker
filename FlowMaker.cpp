#include <iostream>
#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <map>

using namespace std;

const string FLOWS_CSV_FILE = "flows.csv";

bool isValidFileName(const string &fileName){
    string invalidChars = "\\/:*?\"<>|";
    for (char ch : fileName){
        if (invalidChars.find(ch) != string::npos){
            return false;
        }
    }
    if (fileName.empty() || fileName.find_first_not_of(" \t") == string::npos)
    {
        return false;
    }
    return true;
}

enum class ArithmeticOperation {
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Minimum,
    Maximum
};

class FlowStep{
    public:
        virtual void execute() = 0;
        virtual string getType() const = 0;
        virtual string getDescription() const = 0;
        virtual FlowStep *clone() const = 0;
        virtual void reset() {}
        virtual ~FlowStep() {}
};

class TitleStep : public FlowStep{
    private:
        string title;
        string subtitle;
        bool complete = false;
    public:

        TitleStep(const string &title = "Default Title for TitleStep", const string &subtitle = "Default Subtitle for TitleStep") : title(title), subtitle(subtitle) {}

        void reset() override{
            complete = false;
            title = "Default Title for TitleStep";
            subtitle = "Default Subtitle for TitleStep";
        }

        void execute() override{
            cout << "Title: " << title << endl;
            cout << "Subtitle: " << subtitle << endl;
        }

        FlowStep *clone() const override{
            try{
                return new TitleStep(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }

        string getType() const override {return "TitleStep";}
        string getDescription() const override {return "Step with a title and subtitle.";}
        string getTitle() const {return title;}
        string getSubtitle() const {return subtitle;}
        bool getCompleteTitleStep() const {return complete;}
        void setCompleteTitleStep(bool newComplete) {complete = newComplete;}
        void setTitle(const string &newTitle) {title = newTitle;}
        void setSubtitle(const string &newSubtitle) {subtitle = newSubtitle;}
};

class TextStep : public FlowStep{
    private:
        string title;
        string text;
        bool complete = false;
        int stepNumber;
    public:
        TextStep(const string &title = "Default Title for TextStep", const string &text = "Default text for TextStep") : title(title), text(text) {}

        void reset() override{
            complete = false;
            title = "Default Title for TextStep";
            text = "Default text for TextStep";
        }

        void execute() override{
            cout << "Text Title: " << title << endl;
            cout << "Text: " << text << endl;
        }

        FlowStep *clone() const override{
            try{
                return new TextStep(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }

        string getType() const override {return "TextStep";}
        string getDescription() const override {return "Step with a title for the text and text.";}
        string getTitle() const {return title;}
        string getText() const {return text;}
        bool getCompleteTextStep() const {return complete;}
        int getStepNumberTextStep() const {return stepNumber;}
        void setStepNumberTextStep(int newStepNumber) {stepNumber = newStepNumber;}
        void setCompleteTextStep(bool newComplete) {complete = newComplete;}
        void setTitle(const string &newTitle) {title = newTitle;}
        void setText(const string &newText) {text = newText;}
};

class TextInputStep : public FlowStep{
    private:
        string description;
    public:
        TextInputStep(const string &description = "Default Description") : description(description) {}

        void reset() override{
            description = "Default Description";
        }

        void execute() override{
            cout << "Text Input Step Description: " << description << endl;
        }

        FlowStep *clone() const override{
            try{
                return new TextInputStep(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }

        string getType() const override {return "TextInputStep";}
        string getDescription() const override {return ("Step to input the text.\nDescription of the user that created the step: " + description);}
};

class NumberInputStep : public FlowStep{
    private:
        string description;
        double userInput = 0.0;
    public:
        NumberInputStep(const string &description = "Default Number Input Description") : description(description) {}

        void execute() override{
            cout << "Number Input Step Description: " << description << endl;
        }

        void reset() override{
            description = "Default Number Input Description";
            userInput = 0.0;
        }

        FlowStep *clone() const override{
            try{
                return new NumberInputStep(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }

        string getType() const override {return "NumberInputStep";}
        string getDescription() const override {return ("Step to input a number.\nDescription of the user that created this step: " + description);}
        double getUserInput() const {return userInput;}
        void setUserInput(double input) {userInput = input;}
};

template <typename T>
class CalculusStep : public FlowStep{
    private:
        ArithmeticOperation operation;
        vector<NumberInputStep *> numberInputs;
        char operationSymbol;
    public:
        CalculusStep(ArithmeticOperation operation, char operationSymbol) : operation(operation), operationSymbol(operationSymbol) {}

        void addNumberInput(NumberInputStep *inputStep){
            if (inputStep == nullptr){
                throw invalid_argument("Input step pointer is null");
            }
            numberInputs.push_back(inputStep);
        }

        void reset() override{
            operation = ArithmeticOperation::Addition;
            operationSymbol = '+';
            numberInputs.clear();
        }

        void setOperation(ArithmeticOperation op){
            if (op != ArithmeticOperation::Addition && op != ArithmeticOperation::Subtraction &&
                op != ArithmeticOperation::Multiplication && op != ArithmeticOperation::Division &&
                op != ArithmeticOperation::Minimum && op != ArithmeticOperation::Maximum)
            {
                throw invalid_argument("Invalid arithmetic operation");
            }
            operation = op;
        }

        T performCalculation() const{
            T result = 0;
            switch (operation){
            case ArithmeticOperation::Addition:
                for (NumberInputStep *step : numberInputs){
                    result += static_cast<T>(step->getUserInput());
                }
                break;
            case ArithmeticOperation::Subtraction:
                if (!numberInputs.empty()){
                    result = static_cast<T>(numberInputs[0]->getUserInput());
                    for (size_t i = 1; i < numberInputs.size(); ++i){
                        result -= static_cast<T>(numberInputs[i]->getUserInput());
                    }
                }
                break;
            case ArithmeticOperation::Multiplication:
                result = 1;
                for (NumberInputStep *step : numberInputs){
                    result *= static_cast<T>(step->getUserInput());
                }
                break;
            case ArithmeticOperation::Division:
                if (!numberInputs.empty()){
                    result = static_cast<T>(numberInputs[0]->getUserInput());
                    for (size_t i = 1; i < numberInputs.size(); ++i){
                        if (numberInputs[i]->getUserInput() != 0){
                            result /= static_cast<T>(numberInputs[i]->getUserInput());
                        }
                        else{
                            throw runtime_error("Division by zero detected. Skipping.");
                        }
                    }
                }
                break;
            case ArithmeticOperation::Minimum:
                if (!numberInputs.empty()){
                    result = static_cast<T>(numberInputs[0]->getUserInput());
                    for (size_t i = 1; i < numberInputs.size(); ++i){
                        result = min(result, static_cast<T>(numberInputs[i]->getUserInput()));
                    }
                }
                break;
            case ArithmeticOperation::Maximum:
                if (!numberInputs.empty()){
                    result = static_cast<T>(numberInputs[0]->getUserInput());
                    for (size_t i = 1; i < numberInputs.size(); ++i){
                        result = max(result, static_cast<T>(numberInputs[i]->getUserInput()));
                    }
                }
                break;
            }
            return result;
        }

        void execute() override{
            cout << "Performing Calculus Step: ";
            switch (operation){
            case ArithmeticOperation::Addition:
                cout << "Addition" << endl;
                break;
            case ArithmeticOperation::Subtraction:
                cout << "Subtraction" << endl;
                break;
            case ArithmeticOperation::Multiplication:
                cout << "Multiplication" << endl;
                break;
            case ArithmeticOperation::Division:
                cout << "Division" << endl;
                break;
            case ArithmeticOperation::Minimum:
                cout << "Minimum" << endl;
                break;
            case ArithmeticOperation::Maximum:
                cout << "Maximum" << endl;
                break;
            }
            try{
                cout << "Result: " << performCalculation() << endl;
            }catch (const runtime_error &e){
                cout << "Error: " << e.what() << endl;
            }
        }

        FlowStep *clone() const override{
            try{
                return new CalculusStep<T>(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }
        string getType() const override {return "CalculusStep";}
        string getDescription() const override {return "Step to perform arithmetic operations. (+, -, *, /, m (min), M (max))";}
        void setOperationSymbol(char symbol) {operationSymbol = symbol;}
        char getOperationSymbol() const {return operationSymbol;}
        const vector<NumberInputStep *> &getNumberInputs() const {return numberInputs;}
};

class DisplayStep : public FlowStep{
    public:
        DisplayStep() {}

        void execute() override{
            cout << "Displaying the Flow" << endl;
        }

        string getType() const override {return "DisplayStep";}
        string getDescription() const override {return "Displaying the flow.";}

        FlowStep *clone() const override{
            try{
                return new DisplayStep(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }
};

class TextFileInputStep : public FlowStep{
    private:
        string description;
        string fileName;
        bool fileImported = false;
        string fileContent;
    public:
        TextFileInputStep(const string &description = "Default Description") : description(description) {}

        void reset() override{
            fileImported = false;
            fileContent = "";
            fileName = "";
            description = "Default Description";
        }

        void execute() override{
            while (true){
                cout << "Enter the name of the text file (.txt): ";
                getline(cin, fileName);

                if (!isValidFileName(fileName)){
                    cout << "Invalid file name. Please enter a valid file name." << endl;
                }
                else{
                    size_t pos = fileName.find_last_of(".");
                    if (pos == string::npos || fileName.substr(pos) != ".txt")
                    {
                        fileName += ".txt";
                    }
                    break;
                }
            }

            cout << "Entered File Name: " << fileName << endl;

            ifstream inputFile(fileName);
            if (inputFile.is_open()){
                fileImported = true;
                string line;
                try{
                    while (getline(inputFile, line)){
                        fileContent += line + "\n";
                    }
                }catch (const exception &e){
                    cout << "Error reading the file: " << e.what() << endl;
                    fileImported = false;
                }

                inputFile.close();

                if (fileImported){
                    cout << "File imported successfully." << endl;
                }
            }

            else{
                cout << "File not found or unable to open." << endl;
            }
        }

        FlowStep *clone() const override{
            try{
                return new TextFileInputStep(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }

        bool isFileImported() const {return fileImported;}
        string getFileContent() const {return fileContent;}
        string getFileName() const {return fileName;}
        string getType() const override {return "TextFileInputStep";}
        string getDescription() const override {return ("Step to input a text file (.txt).\nDescription of the user that created the step: " + description);}
};

class CSVFileInputStep : public FlowStep{
    private:
        string description;
        string fileName;
        bool fileImported = false;
        vector<vector<string>> csvData;
    public:
        CSVFileInputStep(const string &description = "Default Description") : description(description) {}

        void reset() override{
            description = "Default Description";
            fileName = "";
            fileImported = false;
            csvData.clear();
        }

        void execute() override{
            while (true){
                cout << "Enter the name of the CSV file (.csv): ";
                getline(cin, fileName);

                if (!isValidFileName(fileName)){
                    cout << "Invalid file name. Please enter a valid CSV file name." << endl;
                }

                else{
                    size_t pos = fileName.find_last_of(".");
                    if (pos == string::npos || fileName.substr(pos) != ".csv"){
                        fileName += ".csv";
                    }
                    break;
                }
            }

            cout << "Entered File Name: " << fileName << endl;

            try{
                ifstream inputFile(fileName);
                if (!inputFile.is_open()){
                    cout << "File not found or unable to open." << endl;
                    return;
                }

                fileImported = true;
                csvData.clear();
                string line;
                while (getline(inputFile, line)){
                    stringstream ss(line);
                    vector<string> row;
                    string cell;

                    while (getline(ss, cell, ',')){
                        row.push_back(cell);
                    }

                    csvData.push_back(row);
                }

                inputFile.close();
                cout << "CSV file imported successfully." << endl;
            }catch (const ifstream::failure &e){
                cerr << "Error opening or reading the file: " << e.what() << endl;
                fileImported = false; 
            }
        }

        FlowStep *clone() const override{
            try{
                return new CSVFileInputStep(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }

        string getType() const override {return "CSVFileInputStep";}
        string getDescription() const override {return ("Step to input a CSV file (.csv).\nDescription of the user that created the step: " + description);}
        bool isFileImported() const {return fileImported;}
        const vector<vector<string>> &getCSVData() const {return csvData;}
        string getFileName() const {return fileName;}
};

class OutputStep : public FlowStep{
    private:
        string filename;
        string title;
        string description;
        vector<string> outputData;
    public:
        OutputStep(const string &filename = "Default File Name", const string &title = "Default File Title", const string &description = "Default File Description") : filename(filename), title(title), description(description) {}

        void reset() override{
            description = "Default Description";
            title = "Default Title";
            filename = "Default File Name";
            outputData.clear();
        }

        FlowStep *clone() const override{
            try{
                return new OutputStep(*this);
            }
            catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }

        void setOutputData(const vector<string> &data) {outputData = data;}
        string getFilename() const {return filename;}
        string getTitle() const {return title;}
        string getType() const override {return "OutputStep";}
        string getDescription() const {return "Step to output a text file (.txt).";}
        void setFilename(const string &newFilename) {filename = newFilename;}
        void setTitle(const string &newTitle) {title = newTitle;}
        void setDescription(const string &newDescription) {description = newDescription;}

        void handleFilenameConflict(){
            size_t pos = filename.find_last_of(".");
            if (pos == string::npos || filename.substr(pos) != ".txt"){
                filename += ".txt";
            }
            ifstream file(filename);
            int suffix = 0;
            while (file){
                file.close();
                string newFilename = to_string(suffix) + "_" + filename;
                ifstream newFile(newFilename);
                if (!newFile.is_open()){
                    filename = newFilename;
                    break;
                }
                suffix++;
            }
        }

        void execute() override{
            try{
                handleFilenameConflict();
            }catch (const exception &e){
                cerr << e.what() << endl;
                return;
            }
            try{
                ofstream outputFile(filename);
                if (!outputFile.is_open()){
                    throw runtime_error("Error: Unable to open the output file for writing.");
                }

                outputFile << "Title of the output file: " << title << endl;
                outputFile << "Description of the output file: " << description << endl;
                outputFile << "\n\n";

                for (const string &data : outputData){
                    outputFile << data << endl;
                }

                outputFile.close();
                if (outputFile.fail()){
                    throw runtime_error("Error: Failed to write data to the output file.");
                }
                cout << "Output file '" << filename << "' created successfully." << endl;
            }catch (const exception &e){
                cerr << e.what() << endl;
            }
        }
};

class EndStep : public FlowStep{
    public:
        EndStep() {}

        void execute() override{
            cout << "End of Flow" << endl;
        }

        string getType() const override {return "EndStep";}
        string getDescription() const override {return "End of the flow.";}

        FlowStep *clone() const override {
            try{
                return new EndStep(*this);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error: " << e.what() << endl;
                return nullptr;
            }
        }
};

class Flow{
    private:
        string name;
        vector<FlowStep *> steps;

    public:
        Flow(const string &name) : name(name) {}

        ~Flow(){
            for (FlowStep *step : steps){
                delete step;
            }
        }

        void addStep(FlowStep *step){
            try{
                steps.push_back(step);
            }catch (const bad_alloc &e){
                cerr << "Memory allocation error when adding a step: " << e.what() << endl;
            }
        }

        void displayAvailableSteps() const{
            cout << "Available Steps:" << endl;
            cout << "1. TitleStep: Step with a title and subtitle." << endl;
            cout << "2. TextStep: Step with a title and text." << endl;
            cout << "3. TextInputStep: Step which allows the user to input a title and text." << endl;
            cout << "4. NumberInputStep: Step to input a number." << endl;
            cout << "5. CalculusStep: Step to perform arithmetic operations." << endl;
            cout << "6. DisplayStep: Step which displays the input for each of the steps until now." << endl;
            cout << "7. TextFileInputStep: Step which lets the user to input a .txt file." << endl;
            cout << "8. CSVFileInputStep: Step which lets the user to input a .csv file." << endl;
            cout << "9. OutputStep: Step which lets the user to output a .txt file with the information he desires." << endl;
            cout << "0. EndStep: Step which adds automatically after finishing the flow." << endl;
        }

        void displayFlowSteps() const{
            cout << "\tFlow Steps:" << endl;
            for (size_t i = 0; i < steps.size(); ++i){
                cout << "\t";
                cout << i + 1 << ". " << steps[i]->getType() << endl;
            }
        }

        void run(){
            for (FlowStep *step : steps){
                step->execute();
            }
        }

        string getName() const {return name;}
        const vector<FlowStep *> &getSteps() const {return steps;}
};

void saveFlowToCSV(const Flow &flow){
    try{
        ofstream csvFile(FLOWS_CSV_FILE, ios::app);
        if (!csvFile.is_open()){
            throw runtime_error("Unable to open the CSV file for writing.");
        }

        time_t currentTime = time(nullptr);
        struct tm *timeInfo = localtime(&currentTime);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeInfo);

        csvFile << flow.getName() << "," << timestamp << ",";
        const vector<FlowStep *> &steps = flow.getSteps();
        for (const FlowStep *step : steps){
            csvFile << step->getType() << ",";
        }
        csvFile << "\n";
        csvFile.close();
    }catch (const exception &e){
        cerr << "Error: " << e.what() << endl;
    }
}

void displayFlowInfoFromCSV(){
    try{
        ifstream csvFile(FLOWS_CSV_FILE);
        if (!csvFile.is_open()){
            throw runtime_error("Unable to open the CSV file for reading.");
        }

        string line;
        while (getline(csvFile, line)){
            stringstream ss(line);
            string flowName, timestamp, stepType;

            getline(ss, flowName, ',');
            getline(ss, timestamp, ',');
            cout << "Flow Name: " << flowName << endl;
            cout << "Timestamp: " << timestamp << endl;

            cout << "Steps:" << endl;
            while (getline(ss, stepType, ',')){
                cout << "- " << stepType << endl;
            }
            cout << endl;
        }
        csvFile.close();
    }catch (const exception &e){
        cerr << "Error: " << e.what() << endl;
    }
}

vector<string> readExistingFlowNames(){
    vector<string> existingFlowNames;
    try{
        ifstream csvFile(FLOWS_CSV_FILE);
        if (!csvFile.is_open()){
            throw runtime_error("Unable to open the CSV file for reading.");
        }

        string line;
        while (getline(csvFile, line)){
            stringstream ss(line);
            string flowName;
            getline(ss, flowName, ',');
            existingFlowNames.push_back(flowName);
        }
        csvFile.close();
    }catch (const exception &e){
        cerr << "Error: " << e.what() << endl;
    }
    return existingFlowNames;
}

Flow loadFlowFromCSV(const string &flowName){
    Flow loadedFlow(flowName);
    ifstream csvFile(FLOWS_CSV_FILE);

    if (csvFile.is_open()){
        string line;
        while (getline(csvFile, line)){
            stringstream ss(line);
            string csvFlowName, timestamp, stepType;

            getline(ss, csvFlowName, ',');
            getline(ss, timestamp, ',');

            if (csvFlowName == flowName){
                while (getline(ss, stepType, ',')){
                    try{
                        if (stepType == "TitleStep"){
                            loadedFlow.addStep(new TitleStep());
                        }
                        else if (stepType == "TextStep"){
                            loadedFlow.addStep(new TextStep());
                        }
                        else if (stepType == "TextInputStep"){
                            loadedFlow.addStep(new TextInputStep("Input title, subtitle, title text and text"));
                        }
                        else if (stepType == "NumberInputStep"){
                            loadedFlow.addStep(new NumberInputStep("Input a number"));
                        }
                        else if (stepType == "CalculusStep"){
                            loadedFlow.addStep(new CalculusStep<double>(ArithmeticOperation::Addition, '+'));
                        }
                        else if (stepType == "DisplayStep"){
                            loadedFlow.addStep(new DisplayStep());
                        }
                        else if (stepType == "TextFileInputStep"){
                            loadedFlow.addStep(new TextFileInputStep("Input a .txt file"));
                        }
                        else if (stepType == "CSVFileInputStep"){
                            loadedFlow.addStep(new CSVFileInputStep("Input a .csv file"));
                        }
                        else if (stepType == "OutputStep"){
                            loadedFlow.addStep(new OutputStep());
                        }
                        else if (stepType == "EndStep"){
                            loadedFlow.addStep(new EndStep());
                        }
                        else{
                            cerr << "Warning: Unknown step type '" << stepType << "' encountered and skipped." << endl;
                        }
                    }catch (const exception &e){
                        cerr << "Error while adding step: " << e.what() << endl;
                    }
                }
                break;
            }
        }
        csvFile.close();
    }

    else{
        cerr << "Error: Unable to open the CSV file for reading." << endl;
    }

    return loadedFlow;
}

void deleteFlowFromCSV(const string &flowNameToDelete){ 
    ifstream inputFile(FLOWS_CSV_FILE);
    ofstream outputFile("temp.csv"); 

    if (inputFile.is_open() && outputFile.is_open()){
        string line;
        while (getline(inputFile, line)){
            stringstream ss(line);
            string flowName, timestamp, stepType;

            getline(ss, flowName, ',');

            if (flowName == flowNameToDelete){
                continue;
            }

            outputFile << line << endl;
        }

        inputFile.close();
        outputFile.close();

        if (remove(FLOWS_CSV_FILE.c_str()) != 0){
            cerr << "Error: Unable to delete the original CSV file." << endl;
            return;
        }
        if (rename("temp.csv", FLOWS_CSV_FILE.c_str()) != 0){
            cerr << "Error: Unable to rename the temporary file to the original name." << endl;
        }
    }
    
    else{
        cerr << "Error: Unable to open the CSV file for reading or writing." << endl;
    }
}

class FlowExecutor{
    private:
        Flow &flow;
    public:
        FlowExecutor(Flow &flow) : flow(flow) {}
        void executeFlow(){
            try{
                const vector<FlowStep *> &steps = flow.getSteps();
                vector<string> outputData;
                for (size_t i = 0; i < steps.size(); ++i){
                    FlowStep *currentStep = steps[i];

                    if (currentStep->getType() == "TitleStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y'){
                            TitleStep *titleStep = dynamic_cast<TitleStep *>(currentStep);
                            if (titleStep){
                                titleStep->setCompleteTitleStep(true);
                            }
                        }
                        else{
                            cout << "Step skipped." << endl;
                        }
                    }

                    else if (currentStep->getType() == "TextStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y'){
                            TextStep *textStep = dynamic_cast<TextStep *>(currentStep);
                            if (textStep){
                                textStep->setCompleteTextStep(true);
                                textStep->setStepNumberTextStep(i);
                            }
                        }
                        else{
                            cout << "Step skipped." << endl;
                        }
                    }

                    else if (currentStep->getType() == "TextInputStep")
                    {
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y')
                        {
                            cout << "The text you need to complete:" << endl;
                            string input;
                            bool verify;
                            for (size_t j = 0; j < i; j++)
                            {
                                currentStep = steps[j];
                                if (currentStep->getType() == "TitleStep")
                                {
                                    TitleStep *titleStep = dynamic_cast<TitleStep *>(currentStep);
                                    if (titleStep && titleStep->getCompleteTitleStep())
                                    {
                                        verify = true;
                                        cout << "Enter Title: ";
                                        cin.ignore();
                                        getline(cin, input);
                                        titleStep->setTitle(input);
                                        cout << "Enter Subtitle: ";
                                        getline(cin, input);
                                        titleStep->setSubtitle(input);
                                    }
                                }
                                else if (currentStep->getType() == "TextStep")
                                {
                                    TextStep *textStep = dynamic_cast<TextStep *>(currentStep);
                                    if (textStep && textStep->getCompleteTextStep())
                                    {
                                        verify = true;
                                        cout << "Enter Text Title: ";
                                        cin.ignore();
                                        getline(cin, input);
                                        textStep->setTitle(input);
                                        cout << "Enter Text: ";
                                        getline(cin, input);
                                        textStep->setText(input);
                                    }
                                }
                            }
                            if (!verify)
                            {
                                cout << "No step to input" << endl;
                            }
                        }
                    }

                    else if (currentStep->getType() == "DisplayStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y'){
                            cout << "Display of the input so far:" << endl;
                            bool verify = false;
                            int numberTitle = 0;
                            int numberText = 0;
                            int numberNumber = 0;
                            int numberCalculus = 0;
                            int numberTextFileInput = 0;
                            int numberCSVFileInput = 0;
                            for (size_t k = 0; k < i; k++){
                                FlowStep *previousStep = steps[k];
                                if (previousStep->getType() == "TitleStep"){
                                    TitleStep *titleStep = dynamic_cast<TitleStep *>(previousStep);
                                    if (titleStep){
                                        cout << "Title " << numberTitle + 1 << ": " << titleStep->getTitle() << endl;
                                        cout << "Subtitle " << numberTitle + 1 << ": " << titleStep->getSubtitle() << endl;
                                        numberTitle++;
                                        verify = true;
                                    }
                                }

                                else if (previousStep->getType() == "TextStep"){
                                    TextStep *textStep = dynamic_cast<TextStep *>(previousStep);
                                    if (textStep){
                                        cout << "Text title " << numberText + 1 << ": " << textStep->getTitle() << endl;
                                        cout << "Text " << numberText + 1 << ": " << textStep->getText() << endl;
                                        numberText++;
                                        verify = true;
                                    }
                                }

                                else if (previousStep->getType() == "NumberInputStep"){
                                    NumberInputStep *numberInputStep = dynamic_cast<NumberInputStep *>(previousStep);
                                    if (numberInputStep){
                                        cout << "Number Input " << numberNumber + 1 << ": " << numberInputStep->getUserInput() << endl;
                                        numberNumber++;
                                        verify = true;
                                    }
                                }

                                else if (previousStep->getType() == "CalculusStep"){
                                    CalculusStep<double> *calculusStep = dynamic_cast<CalculusStep<double> *>(previousStep);
                                    if (calculusStep){
                                        char operationSymbol = calculusStep->getOperationSymbol();
                                        cout << "Calculus Step " << numberCalculus + 1 << ": ";

                                        if (operationSymbol == 'm' || operationSymbol == 'M'){
                                            string operationName = (operationSymbol == 'm') ? "min" : "max";
                                            cout << operationName << "(";
                                        }

                                        for (size_t i = 0; i < calculusStep->getNumberInputs().size(); ++i){
                                            cout << calculusStep->getNumberInputs()[i]->getUserInput();
                                            if (i < calculusStep->getNumberInputs().size() - 1){
                                                if (operationSymbol == 'm' || operationSymbol == 'M'){
                                                    cout << ", ";
                                                }
                                                else{
                                                    cout << " " << operationSymbol << " ";
                                                }
                                            }
                                        }

                                        if (operationSymbol == 'm' || operationSymbol == 'M'){
                                            cout << ") = ";
                                        }
                                        else{
                                            cout << " = ";
                                        }
                                        cout << calculusStep->performCalculation() << endl;
                                        numberCalculus++;
                                        verify = true;
                                    }
                                }

                                else if (previousStep->getType() == "TextFileInputStep"){
                                    TextFileInputStep *textFileInputStep = dynamic_cast<TextFileInputStep *>(previousStep);
                                    if (textFileInputStep){
                                        if (textFileInputStep->isFileImported()){
                                            cout << "Text File " << numberTextFileInput + 1 << " name: " << textFileInputStep->getFileName() << endl;
                                            cout << "Text File " << numberTextFileInput + 1 << " content: \n"
                                                << textFileInputStep->getFileContent() << endl;
                                            numberTextFileInput++;
                                            verify = true;
                                        }
                                        else{
                                            cout << "Text File " << numberTextFileInput + 1 << " was not imported successfully." << endl;
                                            numberTextFileInput++;
                                            verify = true;
                                        }
                                    }
                                }

                                else if (previousStep->getType() == "CSVFileInputStep"){
                                    CSVFileInputStep *csvFileInputStep = dynamic_cast<CSVFileInputStep *>(previousStep);
                                    if (csvFileInputStep){
                                        if (csvFileInputStep->isFileImported()){
                                            cout << "CSV File " << numberCSVFileInput + 1 << " name: " << csvFileInputStep->getFileName() << endl;
                                            cout << "CSV File " << numberCSVFileInput + 1 << " content: \n"
                                                << endl;
                                            const vector<vector<string>> &csvData = csvFileInputStep->getCSVData();
                                            for (size_t row = 0; row < csvData.size(); ++row){
                                                for (size_t col = 0; col < csvData[row].size(); ++col){
                                                    cout << csvData[row][col] << ", ";
                                                }
                                                cout << endl;
                                            }
                                            numberCSVFileInput++;
                                            verify = true;
                                        }
                                        else{
                                            cout << "CSV File " << numberCSVFileInput + 1 << " was not imported successfully." << endl;
                                            numberCSVFileInput++;
                                            verify = true;
                                        }
                                    }
                                }
                            }
                            if (verify == false){
                                cout << "Nothing to display." << endl;
                            }
                        }
                    }

                    else if (currentStep->getType() == "NumberInputStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y'){
                            NumberInputStep *numberInputStep = dynamic_cast<NumberInputStep *>(currentStep);
                            if (numberInputStep){
                                double userInput;
                                bool validInput = false;
                                while (!validInput){
                                    cout << "Enter a number: ";
                                    cin >> userInput;
                                    if (cin.fail()){
                                        cin.clear();
                                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                        cerr << "Invalid input. Please enter a valid number." << endl;
                                    }
                                    else{
                                        validInput = true;
                                        numberInputStep->setUserInput(userInput);
                                        cout << "Number entered is: " << userInput << endl;
                                    }
                                }
                            }
                        }
                    }

                    else if (currentStep->getType() == "CalculusStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y'){
                            CalculusStep<double> *calculusStep = dynamic_cast<CalculusStep<double> *>(currentStep);
                            if (calculusStep){
                                cout << "Choose two number inputs for the calculation:" << endl;
                                vector<int> selectedInputs;
                                int inputChoice;
                                bool verifyExistanceNumbers = false;
                                for (size_t j = 0; j < i; ++j){
                                    FlowStep *previousStep = steps[j];
                                    if (previousStep->getType() == "NumberInputStep"){
                                        NumberInputStep *numberInputStep = dynamic_cast<NumberInputStep *>(previousStep);
                                        verifyExistanceNumbers = true;
                                        if (numberInputStep){
                                            cout << "Select Number Input Step " << j + 1 << "? (Number is: " << numberInputStep->getUserInput() << ") (Y/N): ";
                                            char selectInput;
                                            cin >> selectInput;
                                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                            if (selectInput == 'Y' || selectInput == 'y'){
                                                selectedInputs.push_back(j);
                                                if (selectedInputs.size() >= 2){
                                                    break;
                                                }
                                                j = -1;
                                            }
                                        }
                                    }
                                }

                                try{
                                    if (!verifyExistanceNumbers){
                                        throw runtime_error("No number input step from previous steps. Cancelling calculation.");
                                    }
                                    if (selectedInputs.size() != 2){
                                        throw runtime_error("Invalid number of selected inputs. Cancelling calculation.");
                                    }

                                    for (int selectedInput : selectedInputs){
                                        NumberInputStep *inputStep = dynamic_cast<NumberInputStep *>(steps[selectedInput]);
                                        if (inputStep){
                                            calculusStep->addNumberInput(inputStep);
                                        }
                                    }

                                    cout << "Choose the arithmetic operation (+, -, *, /, m (min), M (max)): ";
                                    char operationSymbol;
                                    bool validOperation = false;

                                    while (!validOperation){
                                        cin >> operationSymbol;
                                        cin.ignore();

                                        switch (operationSymbol){
                                        case '+':
                                        case '-':
                                        case '*':
                                        case '/':
                                        case 'm':
                                        case 'M':
                                            validOperation = true;
                                            break;
                                        default:
                                            cout << "Invalid symbol. Please choose a valid arithmetic operation (+, -, *, /, m (min), M (max)): ";
                                        }
                                    }

                                    switch (operationSymbol){
                                    case '+':
                                        calculusStep->setOperation(ArithmeticOperation::Addition);
                                        calculusStep->setOperationSymbol('+');
                                        break;
                                    case '-':
                                        calculusStep->setOperation(ArithmeticOperation::Subtraction);
                                        calculusStep->setOperationSymbol('-');
                                        break;
                                    case '*':
                                        calculusStep->setOperation(ArithmeticOperation::Multiplication);
                                        calculusStep->setOperationSymbol('*');
                                        break;
                                    case '/':
                                        calculusStep->setOperation(ArithmeticOperation::Division);
                                        calculusStep->setOperationSymbol('/');
                                        break;
                                    case 'm':
                                        calculusStep->setOperation(ArithmeticOperation::Minimum);
                                        calculusStep->setOperationSymbol('m');
                                        break;
                                    case 'M':
                                        calculusStep->setOperation(ArithmeticOperation::Maximum);
                                        calculusStep->setOperationSymbol('M');
                                        break;
                                    default:
                                        cout << "Invalid symbol. Defaulting to addition." << endl;
                                        calculusStep->setOperation(ArithmeticOperation::Addition);
                                        calculusStep->setOperationSymbol('+');
                                    }

                                    double result = calculusStep->performCalculation();
                                    cout << "Calculation Result: " << result << endl;
                                }catch (const runtime_error &ex){
                                    cerr << "Error: " << ex.what() << endl;
                                }
                            }
                        }
                    }

                    else if (currentStep->getType() == "TextFileInputStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y'){
                            TextFileInputStep *textFileInputStep = dynamic_cast<TextFileInputStep *>(currentStep);
                            if (textFileInputStep){
                                textFileInputStep->execute();
                            }
                        }
                    }

                    else if (currentStep->getType() == "CSVFileInputStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y'){
                            CSVFileInputStep *csvFileInputStep = dynamic_cast<CSVFileInputStep *>(currentStep);
                            if (csvFileInputStep){
                                csvFileInputStep->execute();
                            }
                        }
                    }

                    else if (currentStep->getType() == "OutputStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Do you want to complete this step? (Y/N): ";
                        char completeStep;
                        cin >> completeStep;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (completeStep == 'Y' || completeStep == 'y'){
                            string filenameOutput;
                            string titleOutput;
                            string descriptionOutput;
                            bool validFileName = false;
                            while (!validFileName){
                                cout << "Enter filename for the output: ";
                                getline(cin, filenameOutput);
                                if (isValidFileName(filenameOutput)){
                                    validFileName = true;
                                }
                                else{
                                    cerr << "Error: Invalid filename. Please enter a valid filename." << endl;
                                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                }
                            }

                            cout << "Enter title for the output: ";
                            getline(cin, titleOutput);

                            cout << "Enter description for the output: ";
                            getline(cin, descriptionOutput);

                            int numberOutputTitleStep = 0;
                            int numberOutputTextStep = 0;
                            int numberOutputNumberStep = 0;
                            int numberOutputCalculusStep = 0;
                            int numberOutputTextFileStep = 0;
                            int numberOutputCsvFileStep = 0;

                            OutputStep *outputStep = dynamic_cast<OutputStep *>(currentStep);
                            if (outputStep){
                                outputData.clear();
                                for (size_t m = 0; m < i; ++m){
                                    FlowStep *previousStep = steps[m];

                                    if (previousStep->getType() == "TitleStep"){
                                        TitleStep *titleStep = dynamic_cast<TitleStep *>(previousStep);
                                        if (titleStep){
                                            cout << "Do you want to output the title and subtitle of the " << titleStep->getType() << " " << numberOutputTitleStep + 1 << "? (Y/N): ";
                                            char outputStep;
                                            cin >> outputStep;
                                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                            if (outputStep == 'Y' || outputStep == 'y'){
                                                outputData.push_back("Title " + to_string(numberOutputTitleStep + 1) + ": " + titleStep->getTitle());
                                                outputData.push_back("Subtitle " + to_string(numberOutputTitleStep + 1) + ": " + titleStep->getSubtitle());
                                            }
                                            numberOutputTitleStep++;
                                        }
                                    }

                                    else if (previousStep->getType() == "TextStep"){
                                        TextStep *textStep = dynamic_cast<TextStep *>(previousStep);
                                        if (textStep){
                                            cout << "Do you want to output the title and text of the " << textStep->getType() << " " << numberOutputTextStep + 1 << "? (Y/N): ";
                                            char outputStep;
                                            cin >> outputStep;
                                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                            if (outputStep == 'Y' || outputStep == 'y'){
                                                outputData.push_back("Text Title " + to_string(numberOutputTextStep + 1) + ": " + textStep->getTitle());
                                                outputData.push_back("Text " + to_string(numberOutputTextStep + 1) + ": " + textStep->getText());
                                            }
                                            numberOutputTextStep++;
                                        }
                                    }

                                    else if (previousStep->getType() == "NumberInputStep"){
                                        NumberInputStep *numberInputStep = dynamic_cast<NumberInputStep *>(previousStep);
                                        if (numberInputStep){
                                            cout << "Do you want to output the number of the " << numberInputStep->getType() << " " << numberOutputNumberStep + 1 << "? (Y/N): ";
                                            char outputStep;
                                            cin >> outputStep;
                                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                            if (outputStep == 'Y' || outputStep == 'y'){
                                                outputData.push_back("Number Input " + to_string(numberOutputNumberStep + 1) + ": " + to_string(numberInputStep->getUserInput()));
                                            }
                                            numberOutputNumberStep++;
                                        }
                                    }

                                    else if (previousStep->getType() == "CalculusStep"){
                                        CalculusStep<double> *calculusStep = dynamic_cast<CalculusStep<double> *>(previousStep);
                                        if (calculusStep){
                                            cout << "Do you want to output the calculus of the " << calculusStep->getType() << " " << numberOutputCalculusStep + 1 << "? (Y/N): ";
                                            char outputStep;
                                            cin >> outputStep;
                                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                            if (outputStep == 'Y' || outputStep == 'y'){
                                                char operationSymbol = calculusStep->getOperationSymbol();
                                                string calculusOutput = "Calculus Result " + to_string(numberOutputCalculusStep + 1) + ": ";

                                                if (operationSymbol == 'm' || operationSymbol == 'M'){
                                                    string operationName = (operationSymbol == 'm') ? "min" : "max";
                                                    calculusOutput += operationName + "(";
                                                }

                                                for (size_t i = 0; i < calculusStep->getNumberInputs().size(); ++i){
                                                    calculusOutput += to_string(calculusStep->getNumberInputs()[i]->getUserInput());
                                                    if (i < calculusStep->getNumberInputs().size() - 1){
                                                        if (operationSymbol == 'm' || operationSymbol == 'M'){
                                                            calculusOutput += ", ";
                                                        }
                                                        else{
                                                            calculusOutput += " " + to_string(operationSymbol) + " ";
                                                        }
                                                    }
                                                }

                                                if (operationSymbol == 'm' || operationSymbol == 'M'){
                                                    calculusOutput += ")";
                                                }
                                                else{
                                                    calculusOutput += " = ";
                                                }

                                                calculusOutput += to_string(calculusStep->performCalculation());
                                                outputData.push_back(calculusOutput);
                                            }
                                            numberOutputCalculusStep++;
                                        }
                                    }

                                    else if (previousStep->getType() == "TextFileInputStep"){
                                        TextFileInputStep *textFileInputStep = dynamic_cast<TextFileInputStep *>(previousStep);
                                        if (textFileInputStep){
                                            cout << "Do you want to output the text contents of the " << textFileInputStep->getType() << " " << numberOutputTextFileStep + 1 << "? (Y/N): ";
                                            char outputStep;
                                            cin >> outputStep;
                                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                            if (outputStep == 'Y' || outputStep == 'y'){
                                                outputData.push_back("Name of the Text File Input " + to_string(numberOutputTextFileStep + 1) + ": " + textFileInputStep->getFileName());
                                                outputData.push_back("Content of the Text File Input " + to_string(numberOutputTextFileStep + 1) + ": ");
                                                outputData.push_back(textFileInputStep->getFileContent());
                                            }
                                            numberOutputTextFileStep++;
                                        }
                                    }

                                    else if (previousStep->getType() == "CSVFileInputStep"){
                                        CSVFileInputStep *csvFileInputStep = dynamic_cast<CSVFileInputStep *>(previousStep);
                                        if (csvFileInputStep){
                                            cout << "Do you want to output the text contents of the" << csvFileInputStep->getType() << " " << numberOutputCsvFileStep + 1 << "? (Y/N): ";
                                            char outputStep;
                                            cin >> outputStep;
                                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                                            if (outputStep == 'Y' || outputStep == 'y'){
                                                outputData.push_back("Name of the CSV File Input " + to_string(numberOutputCsvFileStep + 1) + ": " + csvFileInputStep->getFileName());
                                                outputData.push_back("Content of the CSV File Input " + to_string(numberOutputCsvFileStep + 1) + ": ");
                                                const vector<vector<string>> &csvData = csvFileInputStep->getCSVData();
                                                for (size_t row = 0; row < csvData.size(); ++row){
                                                    string rowContent;
                                                    for (size_t col = 0; col < csvData[row].size(); ++col){
                                                        rowContent += csvData[row][col] + ", ";
                                                    }
                                                    outputData.push_back(rowContent);
                                                }
                                            }
                                            numberOutputCsvFileStep++;
                                        }
                                    }
                                }
                            }

                            outputStep->setFilename(filenameOutput);
                            outputStep->setTitle(titleOutput);
                            outputStep->setDescription(descriptionOutput);
                            outputStep->setOutputData(outputData);
                            outputStep->execute();
                        }
                        else{
                            cout << "Output step skipped." << endl;
                        }
                    }

                    else if (currentStep->getType() == "EndStep"){
                        cout << i + 1 << ". " << currentStep->getType() << ": " << currentStep->getDescription() << endl;
                        cout << "Flow Completed!" << endl;
                        for (FlowStep *step : steps){
                            step->reset();
                        }
                    }
                }
            }
            catch (const exception &ex){
                cerr << "Error: " << ex.what() << endl;
            }
            catch (...){
                cerr << "An unknown error occurred." << endl;
            }
        }
};

int main(){
    try{
        char optionStart;
        Flow myFlow("Default Flow");
        do{
            do{
                cout << "Choose an option from the following:" << endl;
                cout << "1. Create a new flow" << endl;
                cout << "2. Use the flow that has just been created" << endl;
                cout << "3. Save the flow that has just been created" << endl;
                cout << "4. Use a predefined flow" << endl;
                cout << "5. Use a flow created by a user" << endl;
                cout << "6. Delete flows" << endl;
                cout << "0. Exit" << endl;
                cout << "Option: ";
                cin >> optionStart;
                cin.ignore();
            } while (optionStart < '0' || optionStart > '6');

            switch (optionStart){
            case '1':{
                vector<string> existingFlowNames = readExistingFlowNames();
                string flowName;
                bool flowNameExists;
                do{
                    flowNameExists = false;
                    cout << "Enter flow name: ";
                    getline(cin, flowName);
                    for (const string &existingFlowName : existingFlowNames){
                        if (flowName == existingFlowName){
                            flowNameExists = true;
                            cerr << "Error: Flow name already exists. Please choose a different name." << endl;
                            break;
                        }
                    }
                } while (flowNameExists);
                myFlow = Flow(flowName);
                char optionAddStep;
                do{
                    do{
                        myFlow.displayAvailableSteps();
                        cout << "Which step do you want to add? (0-9): ";
                        cin >> optionAddStep;
                    } while (optionAddStep < '0' || optionAddStep > '9');

                    switch (optionAddStep){
                    case '1':
                        myFlow.addStep(new TitleStep());
                        break;
                    case '2':
                        myFlow.addStep(new TextStep());
                        break;
                    case '3':{
                        string description;
                        cout << "Enter description for TextInputStep: ";
                        cin.ignore();
                        getline(cin, description);
                        myFlow.addStep(new TextInputStep(description));
                        break;
                    }
                    case '6':
                        myFlow.addStep(new DisplayStep());
                        break;
                    case '4':{
                        string description;
                        cout << "Enter description for NumberInputStep: ";
                        cin.ignore();
                        getline(cin, description);
                        myFlow.addStep(new NumberInputStep(description));
                        break;
                    }
                    case '5':
                        myFlow.addStep(new CalculusStep<double>(ArithmeticOperation::Addition, '+'));
                        break;
                    case '7':
                    {
                        string description;
                        cout << "Enter description for TextFileInputStep: ";
                        cin.ignore();
                        getline(cin, description);
                        myFlow.addStep(new TextFileInputStep(description));
                        break;
                    }
                    case '8':
                    {
                        string description;
                        cout << "Enter description for CSVFileInputStep: ";
                        cin.ignore();
                        getline(cin, description);
                        myFlow.addStep(new CSVFileInputStep(description));
                        break;
                    }
                    case '9':
                        myFlow.addStep(new OutputStep());
                        break;
                    case '0':
                        myFlow.addStep(new EndStep());
                        cout << "Flow Creation Finished!" << endl;
                        myFlow.displayFlowSteps();
                        break;
                    }
                } while (optionAddStep != '0');
                break;
            }
            case '2':
                if (myFlow.getSteps().empty()){
                    cerr << "Error: No flow has been created yet." << endl;
                }
                else{
                    myFlow.displayFlowSteps();
                    char optionExecuteFlow;
                    cout << "Are you sure you want to execute the flow? (Y/N): ";
                    cin >> optionExecuteFlow;
                    cin.ignore();
                    if (optionExecuteFlow == 'y' || optionExecuteFlow == 'Y'){
                        try{
                            FlowExecutor FlowExecutor(myFlow);
                            FlowExecutor.executeFlow();
                        }
                        catch (const std::exception &e){
                            cerr << "Error during flow execution: " << e.what() << endl;
                        }
                    }
                }
                break;

            case '3':
                if (myFlow.getSteps().empty()){
                    cerr << "Error: No flow has been created yet." << endl;
                }
                else{
                    saveFlowToCSV(myFlow);
                    cout << "Flow saved successfully!" << endl;
                }
                break;
            case '4':{
                while (true){
                    Flow predefinedFlow1("Predefined Flow 1");
                    predefinedFlow1.addStep(new TitleStep());
                    predefinedFlow1.addStep(new TextStep());
                    predefinedFlow1.addStep(new TextInputStep("Input title, subtitle, title text and text"));
                    predefinedFlow1.addStep(new NumberInputStep("Input a number"));
                    predefinedFlow1.addStep(new NumberInputStep("Input a number"));
                    predefinedFlow1.addStep(new CalculusStep<double>(ArithmeticOperation::Addition, '+'));
                    predefinedFlow1.addStep(new DisplayStep());
                    predefinedFlow1.addStep(new TextFileInputStep("Input a .txt file"));
                    predefinedFlow1.addStep(new CSVFileInputStep("Input a .csv file"));
                    predefinedFlow1.addStep(new OutputStep());
                    predefinedFlow1.addStep(new EndStep());

                    Flow predefinedFlow2("Predefined Flow 2");
                    predefinedFlow2.addStep(new TitleStep());
                    predefinedFlow2.addStep(new TextStep());
                    predefinedFlow2.addStep(new TitleStep());
                    predefinedFlow2.addStep(new TextStep());
                    predefinedFlow2.addStep(new TextInputStep("Input title, subtitle, title text and text"));
                    predefinedFlow2.addStep(new TextInputStep("Input title, subtitle, title text and text"));
                    predefinedFlow2.addStep(new DisplayStep());
                    predefinedFlow2.addStep(new OutputStep());
                    predefinedFlow2.addStep(new EndStep());

                    Flow predefinedFlow3("Predefined Flow 3");
                    predefinedFlow3.addStep(new NumberInputStep("Input a number"));
                    predefinedFlow3.addStep(new NumberInputStep("Input a number"));
                    predefinedFlow3.addStep(new NumberInputStep("Input a number"));
                    predefinedFlow3.addStep(new NumberInputStep("Input a number"));
                    predefinedFlow3.addStep(new CalculusStep<double>(ArithmeticOperation::Addition, '+'));
                    predefinedFlow3.addStep(new CalculusStep<double>(ArithmeticOperation::Addition, '+'));
                    predefinedFlow3.addStep(new DisplayStep());
                    predefinedFlow3.addStep(new OutputStep());
                    predefinedFlow3.addStep(new EndStep());

                    Flow predefinedFlow4("Predefined Flow 4");
                    predefinedFlow4.addStep(new TextFileInputStep("Input a .txt file"));
                    predefinedFlow4.addStep(new CSVFileInputStep("Input a .csv file"));
                    predefinedFlow4.addStep(new DisplayStep());
                    predefinedFlow4.addStep(new OutputStep());
                    predefinedFlow4.addStep(new EndStep());

                    FlowExecutor FlowExecutor1(predefinedFlow1);
                    FlowExecutor FlowExecutor2(predefinedFlow2);
                    FlowExecutor FlowExecutor3(predefinedFlow3);
                    FlowExecutor FlowExecutor4(predefinedFlow4);

                    cout << "Available predefined flows:" << endl;
                    cout << "1. " << predefinedFlow1.getName() << endl;
                    predefinedFlow1.displayFlowSteps();
                    cout << "2. " << predefinedFlow2.getName() << endl;
                    predefinedFlow2.displayFlowSteps();
                    cout << "3. " << predefinedFlow3.getName() << endl;
                    predefinedFlow3.displayFlowSteps();
                    cout << "4. " << predefinedFlow4.getName() << endl;
                    predefinedFlow4.displayFlowSteps();
                    cout << "0. Go back to the main menu" << endl;

                    int choice;
                    cout << "Choose a predefined flow (1-4) or go back (0): ";
                    cin >> choice;
                    cin.ignore();

                    switch (choice){
                    case 1:
                        cout << "Using predefined flow: " << predefinedFlow1.getName() << endl;
                        FlowExecutor1.executeFlow();
                        break;
                    case 2:
                        cout << "Using predefined flow: " << predefinedFlow2.getName() << endl;
                        FlowExecutor2.executeFlow();
                        break;
                    case 3:
                        cout << "Using predefined flow: " << predefinedFlow3.getName() << endl;
                        FlowExecutor3.executeFlow();
                        break;
                    case 4:
                        cout << "Using predefined flow: " << predefinedFlow4.getName() << endl;
                        FlowExecutor4.executeFlow();
                        break;
                    case 0:
                        break;
                    default:
                        cerr << "Error: Invalid choice. Please choose a valid predefined flow." << endl;
                        continue;
                    }

                    if (choice >= 0 && choice <= 4){
                        break;
                    }
                }
                break;
            }
            case '5':{
                string fileNameInput;
                cout << "Flows available in CSV:" << endl;

                displayFlowInfoFromCSV();

                while (true){
                    cout << "Enter the name of the flow to use (or enter 0 to exit): ";
                    getline(cin, fileNameInput);

                    if (fileNameInput == "0"){
                        break;
                    }

                    Flow selectedFlow = loadFlowFromCSV(fileNameInput);

                    if (selectedFlow.getSteps().empty()){
                        cerr << "Error: Flow not found. Please enter a valid flow name or enter 0 to exit." << endl;
                    }
                    else
                    {
                        selectedFlow.displayFlowSteps();
                        FlowExecutor flowExecutor(selectedFlow);
                        flowExecutor.executeFlow();
                        break;
                    }
                }
                break;
            }
            case '6':{
                string flowToDelete;
                vector<string> existingFlowNames = readExistingFlowNames();
                if (existingFlowNames.empty()){
                    cerr << "Error: No flows available for deletion." << endl;
                }
                else{
                    cout << "Flows available in CSV:" << endl;
                    displayFlowInfoFromCSV();

                    while (true){
                        cout << "Enter the name of the flow to delete (or enter 0 to exit): ";
                        getline(cin, flowToDelete);

                        if (flowToDelete == "0"){
                            break;
                        }

                        bool flowExists = false;

                        for (const string &existingFlowName : existingFlowNames){
                            if (existingFlowName == flowToDelete){
                                flowExists = true;
                                break;
                            }
                        }

                        if (flowExists){
                            deleteFlowFromCSV(flowToDelete);
                            cout << "Flow '" << flowToDelete << "' deleted successfully!" << endl;
                            break;
                        }
                        else{
                            cerr << "Error: Flow not found. Please enter a valid flow name or enter 0 to exit." << endl;
                        }
                    }
                }
                break;
            }
            case '0':
                cout << "Exiting program..." << endl;
                break;
            }
        } while (optionStart != '0');
    }
    catch (const exception &ex){
        cerr << "Error: " << ex.what() << endl;
    }
    catch (...){
        cerr << "An unknown error occurred." << endl;
    }

    return 0;
}
