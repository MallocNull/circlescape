/*

This has everything needed to show / hide the loading page.

example of how this would work:

user clicks login:
  showLoading() is called;
  feed the progress returns to the the updateProgress(x , y);
  progress is updated until current = remains;
  loading page is then hidden revealing the next page;
*/


var bar;
var percent;
var loading;

window.addEventListener("load", function () {
  loading = document.getElementById("loading-page");
  bar = document.getElementById("loading-page-bar-progress");
});

function showLoading(){
  loading.style.display = "flex";
}

function hideLoading(){
  loading.style.display = "none";
}

function updateProgress(current , remain){
  if(current != remain){
    var percent = (current / remain) * 100 + "%";
    bar.style.width = percent;
    percent = document.getElementById("loading-page-bar-percent").innerHTML = percent;
  } else {
    hideLoading();
  }
}
