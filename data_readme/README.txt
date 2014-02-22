There are five problem instances given in the competition document, each having an Excel file titled "Input_data_set_X.xlsx". 

Please provide your solution data in the same format as presented in the 4 sheets in the output Excel file.
The solution files for each data set should be a separate file called "Output_data_set_x.xlsx".


all data in sheet "Inbound train info" for data sets 2 to 5 are in the following format

day – day 1 to 18 (nDays in code specifies for how many days to generate data)

train_id - train_id

time – arrival time (decimal time, 14.5 – means 14:30)

block – block code

seq_nr – position of a car on a train


Data Set 1: based on illustrative example. 

Data Set 2:  
 
Small data size problem:
1. simplified_output.txt – we use fewer blocks – our simplified problem

2. adj_arrivals_simplified_output.txt – adjusted arrival rates and fewer number of blocks

 
Full data size problem:
1.       standard_output.txt – using historical distributions

2.       adj_arrivals_output.txt – arrival rates are increased by 0.25 (variable rate.adj in code)

 
In case participants need to generate more data sets, a data generation code, data-generation.r, in R with comments is also provided. (R is available for free at http://www.r-project.org/)
