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
				if (resp?.packages.length > 0) {
					let html = '';
					for (const pkg of resp.packages) {
						html += `
						<a class="column searchResultRow" href="/${pkg.package}">
							<div class="name">${pkg.package}</div>
							<div class="description">${pkg.description || '(no description)'}</div>
						</a>
						`;
					}
					$('#searchResults').html(html);
					$('#searchResults').show();
				} else {
					$('#searchResults').hide();
				}
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
	let timeSearch;
	$('.search input').on('input', function(){
		if (timeSearch) {
			clearTimeout(timeSearch);
			timeSearch = null;
		}
		timeSearch = setTimeout(() => search(), 650);
	});
	$(document).on('click', function(event) {
		// Check if the target element of the click event is not a particular subelement
		if (!$(event.target).is('#searchResults')) {
			$('#searchResults').hide();
		}
	});
});