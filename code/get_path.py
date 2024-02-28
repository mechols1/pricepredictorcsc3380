import os


def get_path():
    # Get the home directory
    home_directory = os.path.expanduser('~')

    # Change 'VS_Code/' to the specific folder path you need
    documents_path = os.path.join(home_directory, 'VS_Code/') # Don & Riley needs this
    #documents_path = os.path.join(home_directory, 'github-classroom/hkaiserteaching/')
    return documents_path