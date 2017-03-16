import numpy as np
import matplotlib.pyplot as plt
import csv

def show_hist(arr, title_name, xlabel, show_plot=True):
  hist, bins = np.histogram(arr, 30)
  bins = bins[:-1].astype(np.int)
  if show_plot == True:
    ind = np.arange(1, bins.shape[0] + 1)  # the x locations for the groups
    maxs = hist
    width = 0.35       # the width of the bars
    fig, ax = plt.subplots()
    rects_mid = ax.bar(ind, maxs, .2, color='r')
    ax.set_ylabel('Count')
    ax.set_xlabel(xlabel)
    ax.set_xticks(ind - .5*.2)
    ax.set_xticklabels(bins)
    plt.title(title_name)
    plt.show()
  return hist, bins

def get_last_col_from_csv(matched_points_csv_reader):
  dist_from_links = np.array([float(row[-1]) for row in matched_points_csv_reader])
  return dist_from_links

def visualize_distribution(fname, hist_title, xlabel):
  file_fstream = open(fname, 'rb')
  file_reader = csv.reader(file_fstream)
  points = get_last_col_from_csv(file_reader)
  hist_title += '\nNumber of data points = ' + str(points.shape[0])
  show_hist(points, hist_title, xlabel)
  file_fstream.close()

if __name__ == "__main__":
  visualize_distribution('Partition6467MatchedPoints.csv',
                         'Probe-Point to link distance distribution in matched points result file',
                         'Distance from Link (in metres)')
  visualize_distribution('Partition6467ComputedSlopes.csv',
                         'Slope (in degrees) distribution as computed from matched probes & links',
                         'Slope (in degrees)')
  visualize_distribution('Partition6467LinkDataSlopes.csv',
                         'Slope (in degrees) distribution as obtained from provided Link Data file',
                         'Slope (in degrees)')
  visualize_distribution('Partition6467ComputedVsLinkDataSlopeComparisons.csv',
                         'Distribution of the absolute difference between computed slopes (from matched probes) and actual slopes',
                         '|Actual Link Slope - Computed Slopes (from matched probes)| (in degrees)')