$(document).ready(function () {
	const search = () => {
		$.ajax({
			type: "POST",
			url: '/search',
			data: JSON.stringify({
				search: $('.search input').val()
			}),
			contentType: 'application/json',
			success: (resp) => {
				console.log(resp)
			},
		});
	}
	$('#searchButton').click(() => {
		search();
	});
	$('.search input').keypress(function (e) {
		if (e.which == 13) {
		  search();
		  return false;
		}
	});
});