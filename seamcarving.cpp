
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp> 
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#define  nl '\n'

using namespace std;
using namespace cv;

//default hold duration (in ms)
// const int DEFAULT_HOLD_DURATION = 10;
// int hold_duration = 0;


//Deprecated - DO NOT USE
typedef struct Vec3d {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} color;


bool validate_image_size(int& new_image_width, int& new_image_height, const int& IMAGE_WIDTH, const int& IMAGE_HEIGHT) {
    if (new_image_width < 0 || new_image_width > IMAGE_WIDTH || new_image_height < 0 || new_image_height > IMAGE_HEIGHT) {
        return false;
    }
    return true;
}

string create_output_image_path (string& input_image_path) {
    int i = 0;
    for (i = input_image_path.size() - 1; i >= 0; i--) {
        if (input_image_path[i] == '.') {
            break;
        }
    }
    return input_image_path.substr(0, i) + "_compressed" + input_image_path.substr(i);
}


int get_min_index(long double& a, long double& b, long double& c) {
    // cout << a << " " << b << " " << c << " ";
    if (a <= b) {
        if (a <= c) {
            return -1;
        }
        else {
            return 1;
        }
    }
    else {
        if (b <= c) {
            return 0;
        }
        else {
            return 1;
        }
    }
    // return 0;
}


// **** Deprecated Function -Do NOT USE
void print_image_data(vector<vector<color>>& pixel_data_matrix) {
    int rows = pixel_data_matrix.size();
    int cols = pixel_data_matrix[0].size();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << "(" << (int)pixel_data_matrix[i][j].red << "," << (int)pixel_data_matrix[i][j].green << "," << (int)pixel_data_matrix[i][j].blue << "," << (int)pixel_data_matrix[i][j].alpha << ")"; 
        }
        cout << nl;
    }
    cout << nl << nl << nl;
}


void print_energy_values (vector<vector<long double>>& energy_values) {
    int rows = energy_values.size();
    int cols = energy_values[0].size();
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << energy_values[i][j] << " ";
        }
        cout << nl;
    }
    cout << nl << nl << nl;
}


void calculate_energy_values(vector<vector<long double>>& energy_values, Mat& image_data, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            // int red_col = (int)pixel_data_matrix[i][j].red;
            // int green_col = (int)pixel_data_matrix[i][j].green;
            // int blue_col = (int)pixel_data_matrix[i][j].blue;
            // int del_red_x = pow(abs(((int)pixel_data_matrix[i][(j - 1 + cols) % cols].red) - ((int)pixel_data_matrix[i][(j + 1) % cols].red)), 2);
            // int del_green_x = pow(abs(((int)pixel_data_matrix[i][(j - 1 + cols) % cols].green) - ((int)pixel_data_matrix[i][(j + 1) % cols].green)), 2);
            // int del_blue_x = pow(abs(((int)pixel_data_matrix[i][(j - 1 + cols) % cols].blue) - ((int)pixel_data_matrix[i][(j + 1) % cols].blue)), 2);
            // int total_del_x = del_red_x + del_blue_x + del_green_x;

            Vec3b l_pixel_data = image_data.at<Vec3b>(i, (j - 1 + cols) % cols);
            Vec3b r_pixel_data = image_data.at<Vec3b>(i, (j + 1) % cols);
            Vec3b t_pixel_data = image_data.at<Vec3b>((i - 1 + rows) % rows, j);
            Vec3b b_pixel_data = image_data.at<Vec3b>((i + 1) % rows, j);

            int total_del_x = pow(abs(l_pixel_data[0] - r_pixel_data[0]), 2) + pow(abs(l_pixel_data[1] - r_pixel_data[1]), 2) + pow(abs(l_pixel_data[2] - r_pixel_data[2]), 2);
            int total_del_y = pow(abs(t_pixel_data[0] - b_pixel_data[0]), 2) + pow(abs(t_pixel_data[1] - b_pixel_data[1]), 2) + pow(abs(t_pixel_data[2] - b_pixel_data[2]), 2);


            // int del_red_y = pow(abs(((int)pixel_data_matrix[(i - 1 + rows) % rows][j].red) - ((int)pixel_data_matrix[(i + 1) % rows][j].red)), 2);
            // int del_green_y = pow(abs(((int)pixel_data_matrix[(i - 1 + rows) % rows][j].red) - ((int)pixel_data_matrix[(i + 1) % rows][j].red)), 2);
            // int del_blue_y = pow(abs(((int)pixel_data_matrix[(i - 1 + rows) % rows][j].red) - ((int)pixel_data_matrix[(i + 1) % rows][j].red)), 2);
            // int total_del_y = del_red_y + del_green_y + del_blue_y; 

            energy_values[i][j] = sqrt((long double)total_del_x + (long double)total_del_y);
        }
    }
}


void show_energy_gradient (Mat& image_data) {
    int rows = image_data.rows;
    int cols = image_data.cols;
    long double max_val = sqrt(6) * 255.0;
    vector<vector<long double>> energy_values(rows, vector<long double>(cols));

    calculate_energy_values(energy_values, image_data, rows, cols);
    
    Mat temp_image_data = image_data.clone();
    temp_image_data.create(rows, cols, CV_8UC3);



    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            energy_values[i][j] = energy_values[i][j] / max_val * 255.0;
            temp_image_data.at<Vec3b>(i, j)[0] = (int)energy_values[i][j]; 
            temp_image_data.at<Vec3b>(i, j)[1] = (int)energy_values[i][j]; 
            temp_image_data.at<Vec3b>(i, j)[2] = (int)energy_values[i][j]; 
        }
    }

    if (rows > 1366 || cols > 768) {
        resizeWindow("Energy Gradient", 1366, 768);
    }

    imshow("Energy Gradient", temp_image_data);
    waitKey(10);
}



vector<int> get_vertical_seam (vector<vector<long double>>& energy_values) {
    int rows = energy_values.size();
    int cols = energy_values[0].size();
    
    if (cols == 1) {
        vector<int>v_seam_data(rows, 0);
        return v_seam_data;
    }
    
    vector<int> vertical_seam_data;

    for (int i = 1; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (j == 0) {
                //Col = 0
                energy_values[i][j] = min(energy_values[i - 1][j], energy_values[i - 1][j + 1]) + energy_values[i][j];
            }
            else if (j == cols - 1) {
                //Col = n - 1
                energy_values[i][j] = min(energy_values[i - 1][j], energy_values[i - 1][j - 1]) + energy_values[i][j];
            }
            else {
                energy_values[i][j] = min(min(energy_values[i - 1][j], energy_values[i - 1][j - 1]), energy_values[i - 1][j + 1]) + energy_values[i][j];
            }
        }
    }

    int min_idx = 0;
    long double min_path_cost = DBL_MAX;
    int j = 0;
    for (j = 0; j < cols; j++) {
        if (energy_values[rows - 1][j] < min_path_cost) {
            min_path_cost = energy_values[rows - 1][j];
            min_idx = j;
        }
    }

    int curr_min = min_idx;
    for (int i = rows - 1; i > 0; i--) {
        vertical_seam_data.push_back(curr_min);
        if (curr_min == 0) {
            if (energy_values[i - 1][curr_min] <= energy_values[i - 1][curr_min + 1]) {
                curr_min = curr_min;
            }
            else {
                curr_min = curr_min + 1;
            }
        }
        else if (curr_min == cols - 1) {
            if (energy_values[i - 1][curr_min] <= energy_values[i - 1][curr_min - 1]) {
                curr_min = curr_min;
            }
            else {
                curr_min = curr_min - 1;
            }
        }
        else {
            curr_min = curr_min + get_min_index(energy_values[i - 1][curr_min - 1], energy_values[i - 1][curr_min], energy_values[i - 1][curr_min + 1]);
        }
    }

    vertical_seam_data.push_back(curr_min);


    return vertical_seam_data;

}


void render_vertical_seam (Mat& input_image_data, vector<int>& vertical_seam_data) {
    Mat input_img_copy = input_image_data.clone();

    int rows = input_image_data.rows;
    int cols = input_image_data.cols;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (j == vertical_seam_data[i]) {
                // input_img_copy = black_pixel;
                input_img_copy.at<Vec3b>(i,j)[0] = 0;
                input_img_copy.at<Vec3b>(i,j)[1] = 0;
                input_img_copy.at<Vec3b>(i,j)[2] = 0;
            }
            else {
                input_img_copy.at<Vec3b>(i, j) = input_image_data.at<Vec3b>(i, j);
            }
        }
    }

    if (rows > 1366 || cols > 768) {
        resizeWindow("Fraz Window", 1366, 768);
    }
    imshow("Fraz Window", input_img_copy);
    waitKey(5);

}


void render_horizontal_seam (Mat& input_image_data, vector<int>& horizontal_seam_data) {
    Mat input_img_copy = input_image_data.clone();

    int rows = input_image_data.rows;
    int cols = input_image_data.cols;

    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            if (i == horizontal_seam_data[j]) {
                // input_img_copy = black_pixel;
                input_img_copy.at<Vec3b>(i,j)[0] = 0;
                input_img_copy.at<Vec3b>(i,j)[1] = 0;
                input_img_copy.at<Vec3b>(i,j)[2] = 0;
            }
            else {
                input_img_copy.at<Vec3b>(i, j) = input_image_data.at<Vec3b>(i, j);
            }
        }
    }

    if (rows > 1366 || cols > 768) {
        resizeWindow("Fraz Window", 1366, 768);
    }
    imshow("Fraz Window", input_img_copy);
    waitKey(5);

}


vector<int> get_horizontal_seam (vector<vector<long double>>& energy_values) {
    int rows = energy_values.size();
    int cols = energy_values[0].size();
    
    if (rows == 1) {
        vector<int>h_seam_data(cols, 0);
        return h_seam_data;
    }
    
    vector<int> horizontal_seam_data;


    for (int j = 1; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            if (i == 0) {
                energy_values[i][j] = min(energy_values[i][j - 1], energy_values[i + 1][j - 1]) + energy_values[i][j];
            }
            else if (i == rows - 1) {
                //Col = n - 1
                energy_values[i][j] = min(energy_values[i][j - 1], energy_values[i - 1][j - 1]) + energy_values[i][j];
            }
            else {
                energy_values[i][j] = min(min(energy_values[i - 1][j - 1], energy_values[i][j - 1]), energy_values[i + 1][j - 1]) + energy_values[i][j];
            }
        }
    }

    int min_idx = 0;
    long double min_path_cost = DBL_MAX;
    int i = 0;
    for (i = 0; i < rows; i++) {
        if (energy_values[i][cols - 1] < min_path_cost) {
            min_path_cost = energy_values[i][cols - 1];
            min_idx = i;
        }
    }

    int curr_min = min_idx;
    //Backtrack for seam
    for (int j = cols - 1; j > 0; j--) {
        horizontal_seam_data.push_back(curr_min);

        if (curr_min == 0) {
            if (energy_values[curr_min][j - 1] <= energy_values[curr_min + 1][j - 1]) {
                curr_min = curr_min;
            }
            else {
                curr_min = curr_min + 1;
            }
        }
        else if (curr_min == rows - 1) {
            if (energy_values[curr_min][j - 1] <= energy_values[curr_min - 1][j - 1]) {
                curr_min = curr_min;
            }
            else {
                curr_min = curr_min - 1;
            }
        }
        else {
            curr_min = curr_min + get_min_index(energy_values[curr_min - 1][j - 1], energy_values[curr_min][j - 1], energy_values[curr_min + 1][j - 1]);
        }
    }

    horizontal_seam_data.push_back(curr_min);

    return horizontal_seam_data;
}



bool remove_vertical_seam(Mat& output_image_data, bool enablegui) {
    int rows = output_image_data.rows;
    int cols = output_image_data.cols;

    Mat temp_image_data = output_image_data.clone();

    vector<vector<long double>> energy_values(rows, vector<long double>(cols));

    calculate_energy_values(energy_values, output_image_data, rows, cols);

    vector<int> vertical_seam_data = get_vertical_seam(energy_values);

    reverse(vertical_seam_data.begin(), vertical_seam_data.end());

    if (enablegui) {
        render_vertical_seam(output_image_data, vertical_seam_data);
    }

    output_image_data.create(rows, cols-1, CV_8UC3);

    for (int i = 0, i_new = 0; i < rows; i++, i_new++) {
        for (int j = 0, j_new = 0; j < cols; j++) {
            if (j == vertical_seam_data[i]) {
                continue;
            }
            else {
                output_image_data.at<Vec3b>(i_new, j_new) = temp_image_data.at<Vec3b>(i, j);
                j_new++;
            }
        }
    }

    return true;

}


bool remove_horizontal_seam(Mat& output_image_data, bool enablegui) {
    int rows = output_image_data.rows;
    int cols = output_image_data.cols;

    Mat temp_image_data = output_image_data.clone();

    // vector<vector<color>> image_data_matrix = get_image_data_matrix(output_image_data);
    vector<vector<long double>> energy_values(rows, vector<long double>(cols));

    calculate_energy_values(energy_values, output_image_data, rows, cols);

    vector<int> horizontal_seam_data = get_horizontal_seam(energy_values);

    reverse(horizontal_seam_data.begin(), horizontal_seam_data.end());

    if (enablegui) {
        render_horizontal_seam(output_image_data, horizontal_seam_data);
    }
    output_image_data.create(rows-1, cols, CV_8UC3);

    for (int j = 0, j_new = 0; j < cols; j++, j_new++) {
        for (int i = 0, i_new = 0; i < rows; i++) {
            if (i == horizontal_seam_data[j]) {
                continue;
            }
            else {
                output_image_data.at<Vec3b>(i_new, j_new) = temp_image_data.at<Vec3b>(i, j);
                i_new++;
            }
        }
    }

    return true;
}



// **** --MAIN-- function
int main(int argc, char** argv) 
{ 

    int new_height = 0;
    int new_width = 0;
    bool enablegui = false;
    bool enablegradientmap = false;

    string input_image_path = "";
    // string input_image_path  = "/home/fraz/IIITH_Lab_assignments/dsaps/assignment1/sample_input/sample_input/sample1.jpeg";

    cout << "Enter Image Path: ";
    cin >> input_image_path;

    string output_image_path  = create_output_image_path(input_image_path);
    
    int new_image_width;
    int new_image_height;

    if (argc >= 3) {
        int new_image_width = atoi(argv[1]);
        int new_image_height = atoi(argv[2]);
    }
    else {
        cout << "Enter new width : ";
        cin >> new_image_width;
        cout << "Enter new height : ";
        cin >> new_image_height;
    }

    cout << "Enable GUI mode(0 - NO, 1 -YES)? ";
    cin >> enablegui;
    cout << "Display Gradient Map?(0 - NO, 1 -YES)? ";
    cin >> enablegradientmap;

    cout << "Final Size " << new_image_width << " x " << new_image_height << endl;

    Mat input_image_data = imread(input_image_path, IMREAD_COLOR);
    
    if (input_image_data.empty()) {
        cout << "Unable to read input Image " << input_image_path << nl;
        exit(EXIT_FAILURE);
    }

    Mat output_image_data = input_image_data.clone();


    const int IMAGE_HEIGHT = input_image_data.rows;
    const int IMAGE_WIDTH = input_image_data.cols;
    const int IMAGE_SIZE = input_image_data.size().width * input_image_data.size().height;
    const int IMAGE_DEPTH = input_image_data.depth();
    const int IMAGE_CHANNELS = input_image_data.channels();
    const int IMAGE_TYPE = input_image_data.type();

    if (!validate_image_size(new_image_width, new_image_height, IMAGE_WIDTH, IMAGE_HEIGHT)) {
        cout << "Invalid Image Sizes" << endl;
        exit(EXIT_FAILURE);
    }


    int to_remove_vertical = IMAGE_WIDTH - new_image_width;
    int to_remove_horizontal = IMAGE_HEIGHT - new_image_height;

    cout << "Hor " << to_remove_horizontal << " Ver " << to_remove_vertical << endl;

    namedWindow("Fraz Window", WINDOW_NORMAL);

    if (enablegradientmap) {
        namedWindow("Energy Gradient", WINDOW_NORMAL);
        show_energy_gradient(input_image_data);
    }

    //Removing Vertical Seamlines
    for (int j = 0; j < to_remove_vertical; j++) {
        cout << "Removing Vertical SeamLines (" << j << " of " << to_remove_vertical << ")" << '\r';
        cout.flush();
        remove_vertical_seam(output_image_data, enablegui);
    }
    cout << "Removing Vertical SeamLines (" << to_remove_vertical << " of " << to_remove_vertical << ")" << endl;


    //Removing Horizontal Seamlines
    for (int i = 0; i < to_remove_horizontal; i++) {
        cout << "Removing Horizontal SeamLines (" << i << " of " << to_remove_horizontal << ")" << '\r';
        cout.flush();
        remove_horizontal_seam(output_image_data, enablegui);
    }
    cout << "Removing Horizontal SeamLines (" << to_remove_horizontal << " of " << to_remove_horizontal << ")" << endl;


    // int v = 0;
    // int h = 0;
    // for (v = 0, h = 0; (v < to_remove_vertical) && (h < to_remove_horizontal); v++, h++) {
    //     remove_vertical_seam(output_image_data, enablegui);
    //     v++;
    //     remove_horizontal_seam(output_image_data, enablegui);
    //     h++;
    // }

    // cout << "Removed ver " << v << " hor " << h << endl; 

    // while (v < to_remove_vertical) {
    //     v++;
    //     remove_vertical_seam(output_image_data, enablegui);
    // }
    // while (h < to_remove_horizontal) {
    //     h++;
    //     remove_horizontal_seam(output_image_data, enablegui);
    // }

    // cout << "FInal removed ver " << v << " hor " << h << endl; 

    imshow("Fraz Window", output_image_data);


    int k = waitKey(0);

    if (k) {
        imwrite(output_image_path, output_image_data);
    }

    destroyAllWindows();

    return 0; 
}
